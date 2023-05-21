/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include "Utils.hpp"

class ResourcePropertyCodeGen : public kodgen::MacroPropertyCodeGen
{
public:
	ResourcePropertyCodeGen() noexcept :
		kodgen::MacroPropertyCodeGen("Resource", kodgen::EEntityType::Field)
	{}

	virtual bool preGenerateCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */, kodgen::MacroCodeGenEnv& env) noexcept override
	{
		auto const& field = reinterpret_cast<kodgen::FieldInfo const&>(entity);

		std::string errorMessage;

		//Can't have * and & at the same time
		if (field.type.getName(false, false, true) != "Darius::ResourceManager::ResourceRef" || !kodgen::TypeInfo::isTemplateTypename(field.type.getCanonicalName()))
		{
			errorMessage = "Resource field has to be wrapped in D_RESOURCE::ResourceRef. (" + field.name + " of type: " + field.type.getName(false, false, true) + ")";
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
		kodgen::FieldInfo const& field = reinterpret_cast<kodgen::FieldInfo const&>(entity);

		auto accessSpecifier = field.accessSpecifier;
		std::string accessSpecifierStr;

		switch (accessSpecifier)
		{
		case kodgen::EAccessSpecifier::Invalid:
			return false;
		case kodgen::EAccessSpecifier::Public:
			accessSpecifierStr = "public:";
			break;
		case kodgen::EAccessSpecifier::Protected:
			accessSpecifierStr = "protected:";
			break;
		case kodgen::EAccessSpecifier::Private:
			accessSpecifierStr = "private:";
			break;
		default:
			return false;
		}


		std::string fieldOfficialName = field.name.at(0) == 'm' ?
				field.name.substr(1, field.name.size() - 1) :
				field.name;

		std::string resourceType;

		{
			std::string trimmedName = field.type.getName(true, true, false);
			auto tempPos = trimmedName.find("<");
			resourceType = trimmedName.substr(tempPos + 1, trimmedName.size() - tempPos - 2);
		}

		inout_result += "public:" + env.getSeparator();
		inout_result += "INLINE void Set" + fieldOfficialName + "(D_RESOURCE::ResourceHandle handle)" + env.getSeparator();
		inout_result += "{" + env.getSeparator();
		inout_result += "\tmChangeSignal();" + env.getSeparator();
		inout_result += "\t_Set" + fieldOfficialName + "(handle);" + env.getSeparator();
		inout_result += "}" + env.getSeparator();

		inout_result += "INLINE " + resourceType + " const* Get" + fieldOfficialName + "() const" + env.getSeparator();
		inout_result += "{" + env.getSeparator();
		inout_result += "\treturn " + field.name + ".Get();" + env.getSeparator();
		inout_result += "}" + env.getSeparator();

		if (property.arguments.size() == 0 || (property.arguments.size() > 0 && property.arguments[0] != "false"))
		{
			inout_result += accessSpecifierStr + env.getSeparator();
			inout_result += "INLINE void _Set" + fieldOfficialName + "(D_RESOURCE::ResourceHandle handle)" + env.getSeparator();
			inout_result += "{" + env.getSeparator();
			inout_result += "\t" + field.name + " = D_RESOURCE::GetResource<" + resourceType + ">(handle, *this);" + env.getSeparator();
			inout_result += "}" + env.getSeparator();
		}

		inout_result += "private:" + env.getSeparator();
		inout_result += "INLINE D_CORE::Uuid __Get" + fieldOfficialName + "_UUID() const" + env.getSeparator();
		inout_result += "{" + env.getSeparator();
		inout_result += "\treturn " + field.name + ".IsValid() ? " + field.name + "->GetUuid() : D_CORE::Uuid();" + env.getSeparator();
		inout_result += "}" + env.getSeparator();

		inout_result += "INLINE void __Set" + fieldOfficialName + "_UUID(D_CORE::Uuid uuid)" + env.getSeparator();
		inout_result += "{" + env.getSeparator();
		inout_result += "\t_Set" + fieldOfficialName + "(*D_RESOURCE::GetResource<" + resourceType + ">(uuid, *GetGameObject())); " + env.getSeparator();
		inout_result += "}" + env.getSeparator();
		return true;
	}

};