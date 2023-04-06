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

bool IsResourceField(kodgen::FieldInfo const& field)
{
	for (auto const& prop : field.properties)
		if (prop.name == "Resource")
			return true;
	return false;
}

void GetFieldInfo(kodgen::FieldInfo const& field, bool& isConst, bool& isSerializable, bool& isResource)
{
	isSerializable = isResource = false;

	for (auto const& prop : field.properties)
	{
		if (prop.name == "Serialize")
		{
			isSerializable = true;
			continue;
		}
		else if (prop.name == "Resource")
		{
			isResource = true;
			continue;
		}
	}

	isConst = IsFieldConst(field);
}
