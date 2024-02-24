#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

namespace Simple
{
	struct BoundingBox3D final
	{
		Math::Vector3f min;
		Math::Vector3f max;
	};

	struct BoundingBox2D final
	{
		Math::Vector2f min;
		Math::Vector2f max;
	};
}