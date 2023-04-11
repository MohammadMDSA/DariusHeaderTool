#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include "Utils.hpp"

class ReflectionEnumCodeGen : public kodgen::MacroPropertyCodeGen
{

public:
	ReflectionEnumCodeGen() noexcept :
		kodgen::MacroPropertyCodeGen("Serialize", kodgen::EEntityType::Enum)
	{}

	virtual int getGenerationOrder() const noexcept override
	{
		return -1;
	}

	virtual bool generateSourceFileHeaderCodeForEntity(kodgen::EntityInfo const& entity,
		kodgen::Property const& property,
		std::uint8_t			propertyIndex,
		kodgen::MacroCodeGenEnv& env,
		std::string& inout_result) noexcept override
	{
		kodgen::EnumInfo const& enumInfo = reinterpret_cast<kodgen::EnumInfo const&>(entity);

		if (enumInfo.enumValues.size() <= 0)
			return false;

		inout_result += "#include <rttr/registration.h>\n";
		inout_result += "RTTR_REGISTRATION_PFX(" + enumInfo.name + ") \n";
		inout_result += "{\n";
		inout_result += "rttr::registration::enumeration<" + enumInfo.getFullName() + ">(\"" + enumInfo.getFullName() + "\")";
		inout_result += "(";

		bool isFirst = true;

		for (auto const& enumVal : enumInfo.enumValues)
		{

			//inout_result += "\n\t.property_readonly(\"" + fieldName + ";
			if (!isFirst)
				inout_result += ",";

			inout_result += "\n\trttr::value(\"\", " + enumVal.getFullName() + ")";

			isFirst = false;
		}

		inout_result += ");\n}\n";

		inout_result += "\n}\n";

		return true;
	}
};
