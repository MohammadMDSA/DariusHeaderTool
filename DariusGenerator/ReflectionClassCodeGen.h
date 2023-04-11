#pragma once

#include <string>

#include "ReflectionBaseCodeGen.h"


#include "Utils.hpp"

class ReflectionClassCodeGen : public ReflectionBaseCodeGen
{

public:
	ReflectionClassCodeGen() noexcept :
		ReflectionBaseCodeGen(kodgen::EEntityType::Class)
	{}

	virtual int getGenerationOrder() const noexcept override
	{
		return -2;
	}

};
