#pragma once
#include "Engine/Math/Transform.hpp"

namespace ECS
{
	struct TransformComponent final
	{
		Math::Transform transform;
	};
}

#include "Engine/ECS/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(TransformComponent);
	EXPOSE_VARIABLE(&TransformComponent::transform);
}