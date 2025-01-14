#pragma once

namespace ECS
{
	struct BasicPlayerComponent final
	{
		float moveSpeed = 1.0f;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(BasicPlayerComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&BasicPlayerComponent::moveSpeed);
}