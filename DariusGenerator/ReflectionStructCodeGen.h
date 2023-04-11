#pragma once

#include <string>

#include "ReflectionBaseCodeGen.h"


#include "Utils.hpp"

class ReflectionStructCodeGen : public ReflectionBaseCodeGen
{

public:
	ReflectionStructCodeGen() noexcept :
		ReflectionBaseCodeGen(kodgen::EEntityType::Struct)
	{}


};
