#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include "Utils.hpp"

class RegistrationBaseCodeGen : public kodgen::MacroPropertyCodeGen
{

public:
	RegistrationBaseCodeGen(kodgen::EEntityType entityType) noexcept :
		kodgen::MacroPropertyCodeGen("Reg", entityType)
	{}

	virtual int getGenerationOrder() const noexcept override
	{
		return -1;
	}

	virtual bool preGenerateCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, std::uint8_t propertyIndex, kodgen::MacroCodeGenEnv& env) noexcept override
	{
		kodgen::StructClassInfo const& safeClass = reinterpret_cast<kodgen::StructClassInfo const&>(entity);
		kodgen::StructClassInfo& clazz = const_cast<kodgen::StructClassInfo&>(safeClass);

		bool foundCodeGenIdentifier = false;
		for (auto const& innerClass : clazz.nestedClasses)
		{
			if (innerClass->name == "__CodeGenIdentifier__")
			{
				foundCodeGenIdentifier = true;
				clazz.codeGenIdentifierLine = innerClass->line;
			}
		}

		if (!foundCodeGenIdentifier)
		{
			std::string entityType = clazz.entityType == kodgen::EEntityType::Class ? " class " : " struct ";
			env.getLogger()->log("Could not find GENERATED_CODE() macro in the class body for " + entityType + clazz.getFullName(), kodgen::ILogger::ELogSeverity::Error);
		}
		return foundCodeGenIdentifier;
	}
};

class RegistrationClassCodeGen : public RegistrationBaseCodeGen
{
public:
	RegistrationClassCodeGen() noexcept :
		RegistrationBaseCodeGen(kodgen::EEntityType::Class)
	{}
};

class RegistrationStructCodeGen : public RegistrationBaseCodeGen
{
public:
	RegistrationStructCodeGen() noexcept :
		RegistrationBaseCodeGen(kodgen::EEntityType::Struct)
	{}
};
