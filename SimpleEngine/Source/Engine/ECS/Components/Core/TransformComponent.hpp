#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Reflection.hpp"

namespace ECS
{
	struct TransformComponent final
	{
		Math::Transform transform;
	};

	REGISTER_COMPONENT(TransformComponent);
	REGISTER_PROPERTY(&TransformComponent::transform);
}