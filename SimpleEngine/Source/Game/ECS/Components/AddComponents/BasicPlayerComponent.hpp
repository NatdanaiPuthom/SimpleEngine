#pragma once
#include "Engine/Math/Vector3.hpp"

namespace ECS
{
	struct BasicPlayerComponent final
	{
		float moveSpeed = 1.0f;
		Math::Vector3f cameraOffsetDistance = { 0.0f, 0.0f, 0.0f };
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(BasicPlayerComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&BasicPlayerComponent::moveSpeed);
	REGISTER_AND_EXPOSE_PROPERTY(&BasicPlayerComponent::cameraOffsetDistance);
}