#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include "Utils.hpp"

class ReflectionClassCodeGen : public kodgen::MacroPropertyCodeGen
{

public:
	ReflectionClassCodeGen() noexcept :
		kodgen::MacroPropertyCodeGen("Serialize", kodgen::EEntityType::Class)
	{}

	virtual int getGenerationOrder() const noexcept override
	{
		return -1;
	}

	virtual bool generateClassFooterCodeForEntity(kodgen::EntityInfo const& entity,
		kodgen::Property const& property,
		std::uint8_t			propertyIndex,
		kodgen::MacroCodeGenEnv& env,
		std::string& inout_result) noexcept override
	{
		inout_result += "RTTR_REGISTRATION_FRIEND \\\n";

		return true;
	}

	virtual bool generateSourceFileHeaderCodeForEntity(kodgen::EntityInfo const& entity,
		kodgen::Property const& property,
		std::uint8_t			propertyIndex,
		kodgen::MacroCodeGenEnv& env,
		std::string& inout_result) noexcept override
	{
		kodgen::StructClassInfo const& clazz = reinterpret_cast<kodgen::StructClassInfo const&>(entity);

		if (clazz.properties.size() <= 0)
			return false;

		inout_result += "#include <rttr/registration.h>\n";
		inout_result += "RTTR_REGISTRATION \n";
		inout_result += "{\n";
		inout_result += "rttr::registration::class_<" + clazz.getFullName() + ">(\"" + clazz.getFullName() + "\")";

		for (auto const& field : clazz.fields)
		{
			auto fieldName = field.name[0] == *"m" ? field.name.substr(1, field.name.size() - 1) : field.name;

			// Checking for const descriptor
			if (IsFieldConst(field))
				inout_result += "\n\t.property_readonly(\"" + fieldName + "\", &" + field.getFullName() + ")";
			else
			{
				inout_result += "\n\t.property(\"" + fieldName + "\", &" + field.getFullName() + ")";
				// Serializable
				if (!IsFieldSerializable(field))
					inout_result += " (rttr::metadata(\"NO_SERIALIZE\", true))";
			}
		}

		inout_result += ";";

		inout_result += "\n}\n";

		return true;
	}
};
