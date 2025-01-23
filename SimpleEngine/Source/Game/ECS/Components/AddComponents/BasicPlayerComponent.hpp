#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Math/Vector3.hpp"

namespace ECS
{
	COMPONENT(BasicPlayerComponent) final
	{
		float moveSpeed = 1.0f;
		Math::Vector3f cameraOffsetDistance = { 0.0f, 0.0f, 0.0f };
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&BasicPlayerComponent::moveSpeed);
	REGISTER_AND_EXPOSE_PROPERTY(&BasicPlayerComponent::cameraOffsetDistance);
}