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
			return true;

		bool isResourceCLass = false;

		// Check if the class is resource (whether it has "Resource" property)
		if (std::find_if(clazz.properties.begin(), clazz.properties.end(), [](auto const& prop) { return prop.name == "Resource"; }) != clazz.properties.end())
			isResourceCLass = true;

		inout_result += "#include <rttr/registration.h>\n";
		inout_result += "RTTR_REGISTRATION \n";
		inout_result += "{\n";
		inout_result += "rttr::registration::class_<" + clazz.getFullName() + ">(\"" + clazz.getFullName() + "\")";

		if (isResourceCLass)
		{
			inout_result += "(rttr::metadata(\"RESOURCE\", true))";
		}

		// Adding reflection registration for actual fields
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

				if (!isSerializable || isResource)
				{
					bool isFirst = true;

					inout_result += "(";

					// Serializable
					if (!isSerializable)
					{
						inout_result += "\n\t\trttr::metadata(\"NO_SERIALIZE\", true)";
						isFirst = false;
					}

					if (isResource)
					{
						if (!isFirst)
							inout_result += ",";
						inout_result += "\n\t\trttr::metadata(\"RESOURCE\", true)";
						isFirst = false;
					}

					inout_result += ")";
				}
			}
		}

		// Adding virtual fields through getter and setter
		for (auto const& arg : property.arguments)
		{
			bool isSerializable = true;
			bool isBoolean = false;

			short offset = 0;

			if (arg[offset] == '_')
			{
				isSerializable = false;
				offset++;
			}

			if (arg[offset] == 'b')
			{
				isBoolean = true;
				offset++;
			}

			std::string propName = arg.substr(offset, arg.size() - offset);

			inout_result += "\n\t.property(\"" + propName + "\", &" + clazz.getFullName();
			inout_result += isBoolean ? "::Is" : "::Get";
			inout_result += propName + ", &" + clazz.getFullName() + "::Set" + propName + ")";

			// Serializable
			if (!isSerializable)
				inout_result += " (rttr::metadata(\"NO_SERIALIZE\", true))";
		}

		inout_result += ";";

		inout_result += "\n}\n";

		return true;
	}
};
