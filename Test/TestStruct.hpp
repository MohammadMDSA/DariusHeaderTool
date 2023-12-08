#pragma once

#include "Utils/Common.hpp"

#include "Generated/TestStruct.generated.hpp"

#define BODY_MACRO_COMBINE_INNER(A,B,C,D) A##B##C##D
#define BODY_MACRO_COMBINE(A,B,C,D) BODY_MACRO_COMBINE_INNER(A,B,C,D)
#if CODEGEN_BUILD
#define GENERATED_BODY() class DClass() __CodeGenIdentifier__;
#else
#define GENERATED_BODY() BODY_MACRO_COMBINE(CURRENT_FILE_ID,_,__LINE__,_GENERATED_BODY)
#endif

namespace Darius::Renderer::Light
{
	constexpr unsigned int		MaxNumDirectionalLight = 6;
	constexpr unsigned int		MaxNumPointLight = 125;
	constexpr unsigned int		MaxNumSpotLight = 125;
	constexpr unsigned int		MaxNumLight = MaxNumDirectionalLight + MaxNumPointLight + MaxNumSpotLight;

	enum class DEnum(Serialize) LightSourceType
	{
		DirectionalLight,
			PointLight,
			SpotLight
	};

	struct DStruct(Serialize) LightData
	{
		GENERATED_BODY();

		DField(Serialize)
			bool	Color;

		bool	Direction;				// Directional/Spot light only

		bool	Position;  // Point light only

		DField(Serialize)
			float				Intencity = 1.f;							// Point/Spot light only

		DField(Serialize)
			float				Range = 10.f;								// Point/Spot light only

		bool	SpotAngles;				// Spot light only
		bool	ShadowMatrix;
		bool				CastsShadow = true;
		bool	padding;
	};
}
