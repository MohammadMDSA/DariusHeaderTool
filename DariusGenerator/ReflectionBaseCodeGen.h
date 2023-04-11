#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include "Utils.hpp"

class ReflectionBaseCodeGen : public kodgen::MacroPropertyCodeGen
{

public:
	ReflectionBaseCodeGen(kodgen::EEntityType entityType) noexcept :
		kodgen::MacroPropertyCodeGen("Serialize", entityType)
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
		kodgen::StructClassInfo const& clazz = reinterpret_cast<kodgen::StructClassInfo const&>(entity);

		inout_result += "RTTR_REGISTRATION_FRIEND " + env.getSeparator();
		inout_result += "RTTR_ENABLE(";

		bool firstParent = true;

		for (auto const& parent : clazz.parents)
		{
			if (!firstParent)
				inout_result += ", ";

			firstParent = false;
			inout_result += parent.type.getCanonicalName();
		}
		inout_result += ") " + env.getSeparator();

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
			bool isConst;
			bool isSerializable;
			bool isResource;

			GetFieldInfo(field, isConst, isSerializable, isResource);

			auto fieldName = field.name[0] == 'm' ? field.name.substr(1, field.name.size() - 1) : field.name;

			// Checking for const descriptor
			if (isConst)
				inout_result += "\n\t.property_readonly(\"" + fieldName + "\", &" + field.getFullName() + ")";
			else
			{
				if (isResource)
				{
					inout_result += "\n\t.property(\"" + fieldName + "\", &" + clazz.getFullName() + "::__Get" + fieldName + "_UUID, &" + clazz.getFullName() + "::__Set" + fieldName + "_UUID)";
				}
				else
				{
					inout_result += "\n\t.property(\"" + fieldName + "\", &" + field.getFullName() + ")";
				}
				// Serializable
				if (!isSerializable)
					inout_result += " (rttr::metadata(\"NO_SERIALIZE\", true))";
			}
		}

		inout_result += ";";

		inout_result += "\n}\n";

		return true;
	}
};
