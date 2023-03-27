#pragma once

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

bool IsFieldConst(kodgen::FieldInfo const& field)
{
	static const auto constValueFlag = kodgen::ETypeDescriptor::Const | kodgen::ETypeDescriptor::Value;

	for (auto const& part : field.type.typeParts)
		if ((part.descriptor & constValueFlag) == constValueFlag)
		{
			return true;
		}

	return false;
}

bool IsFieldSerializable(kodgen::FieldInfo const& field)
{
	for (auto const& prop : field.properties)
		if (prop.name == "Serialize")
			return true;
	return false;
}
