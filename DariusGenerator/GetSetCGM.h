#pragma once

#include <Kodgen/CodeGen/Macro/MacroCodeGenModule.h>

#include "GetPropertyCodeGen.h"
#include "SetPropertyCodeGen.h"
#include "ReflectionClassCodeGen.h"
#include "ReflectionStructCodeGen.h"
#include "ReflectionEnumCodeGen.h"
#include "ResourcePropertyCodeGen.h"

class GetSetCGM : public kodgen::MacroCodeGenModule
{
	private:
		GetPropertyCodeGen	_getPropertyCodeGen;
		SetPropertyCodeGen	_setPropertyCodeGen;
		ReflectionClassCodeGen _reflectionClassCodeGen;
		ReflectionStructCodeGen _reflectionStructCodeGen;
		ReflectionEnumCodeGen _reflectionEnumCodeGen;
		ResourcePropertyCodeGen _resourcePropertyCodeGen;

	public:
		GetSetCGM() noexcept
		{
			addPropertyCodeGen(_getPropertyCodeGen);
			addPropertyCodeGen(_setPropertyCodeGen);
			addPropertyCodeGen(_reflectionClassCodeGen);
			addPropertyCodeGen(_reflectionStructCodeGen);
			addPropertyCodeGen(_reflectionEnumCodeGen);
			addPropertyCodeGen(_resourcePropertyCodeGen);
		}

		GetSetCGM(GetSetCGM const&):
			GetSetCGM() //Call the default constructor to add the copied instance its own property references
		{
		}

		virtual GetSetCGM* clone() const noexcept override
		{
			return new GetSetCGM(*this);
		}
};