#pragma once
#include "Engine/Reflection.hpp"

namespace ECS
{
	struct NullComponent final
	{
		bool isNull = true;
	};

	REGISTER_COMPONENT(NullComponent);
}