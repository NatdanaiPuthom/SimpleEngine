#pragma once

namespace ECS
{
	struct BloomComponent
	{
		char bloom = '\0';
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(BloomComponent);
}