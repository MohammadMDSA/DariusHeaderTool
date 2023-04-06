/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <string>

#include "Kodgen/CodeGen/PropertyCodeGen.h"

class SetPropertyCodeGen : public kodgen::MacroPropertyCodeGen
{
public:
	SetPropertyCodeGen() noexcept :
		kodgen::MacroPropertyCodeGen("Set", kodgen::EEntityType::Field)
	{}

	virtual bool preGenerateCodeForEntity(kodgen::EntityInfo const& /* entity */, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */, kodgen::MacroCodeGenEnv& env) noexcept override
	{
		std::string errorMessage;

		//Check that Set property arguments are valid
		if (property.arguments.size() > 1)
		{
			errorMessage = "Set property can't take more than one argument.";
		}
		else
		{
			//Check that Get property arguments are valid
			for (std::string const& arg : property.arguments)
			{
				if (arg != "inline" && arg != "explicit")
				{
					errorMessage = "Get property only accepts 'inline', and 'explicit argument.";
					break;
				}
			}
		}

		if (!errorMessage.empty())
		{
			//Log error message and abort generation
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log(errorMessage, kodgen::ILogger::ELogSeverity::Error);
			}

			return false;
		}

		//If arguments are valid, dispatch the generation call normally
		return true;
	}

	virtual bool generateClassFooterCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */,
		kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
	{
		kodgen::FieldInfo const& field = static_cast<kodgen::FieldInfo const&>(entity);

		//Can't generate any setter if the field is originally const qualified
		if (field.type.typeParts.back().descriptor == kodgen::ETypeDescriptor::Const)
		{
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log("Can't generate setter for the field " + entity.getFullName() + " because it is const qualified. Abort generation.", kodgen::ILogger::ELogSeverity::Error);
			}

			return false;
		}

		bool isInline = false;

		// Extracting property arguments
		for (std::string const& subprop : property.arguments)
		{
			if (subprop.at(0) == 'i')			// inline
			{
				isInline = true;
			}
		}


		std::string paramName = "_v";

		std::string preTypeQualifiers = "";


		std::string methodName;

		// Remove 'm' for member indicator
		if (field.name.at(0) == *"m")
			methodName = field.name.substr(1, field.name.size() - 1);
		else
			methodName = field.name;

		//Upper case the first field info char if applicable
		methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0))));
		methodName.insert(0, "Set");
		methodName += "(";

		methodName += field.type.getName();
		methodName += ((field.type.sizeInBytes == 0u || field.type.sizeInBytes > 4u) &&
			!(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::Ptr) &&
			!(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::LRef)) ? " const& " : " ";
		methodName += paramName;

		methodName += ")";


		if (isInline)
		{
			preTypeQualifiers = "INLINE ";
		}
		if (field.isStatic)
		{
			preTypeQualifiers += "static ";
		}

		inout_result += "public: " + env.getSeparator();

		if (isInline)
			inout_result += preTypeQualifiers + "void " + methodName + " { " + field.name + " = " + paramName + "; }" + env.getSeparator();
		else
			inout_result += preTypeQualifiers + "void " + methodName + ";" + env.getSeparator();


		return true;
	}

	virtual bool generateSourceFileHeaderCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */,
		kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
	{
		kodgen::FieldInfo const& field = static_cast<kodgen::FieldInfo const&>(entity);


		bool				isInline = false;
		bool				isExplicit = false;

		// Extracting property arguments
		for (std::string const& subprop : property.arguments)
		{
			if (subprop.at(0) == 'i')			// inline
			{
				// Not generating setter if it is marked as inline
				return true;
			}
			else if (subprop.at(0) == 'e')		// explicit
			{
				// Not generating setter if it is marked as explicit
				return true;
			}
		}


		std::string paramName = "_v";

		std::string preTypeQualifiers;

		std::string methodName;

		// Remove 'm' for member indicator
		if (field.name.at(0) == *"m")
			methodName = field.name.substr(1, field.name.size() - 1);
		else
			methodName = field.name;

		//Upper case the first field info char if applicable
		methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0))));
		methodName.insert(0, "Set");
		methodName += "(";

		methodName += field.type.getName();
		methodName += ((field.type.sizeInBytes == 0u || field.type.sizeInBytes > 4u) &&
			!(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::Ptr) &&
			!(field.type.typeParts.back().descriptor & kodgen::ETypeDescriptor::LRef)) ? " const& " : " ";
		methodName += paramName;

		methodName += ")";

		inout_result += preTypeQualifiers + "void " + entity.outerEntity->getFullName() + "::" + methodName + " { " + field.name + " = " + paramName + "; }" + env.getSeparator();

		return true;
	}
};