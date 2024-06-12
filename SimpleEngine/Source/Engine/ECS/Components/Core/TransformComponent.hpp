#pragma once
#include "Engine/Math/Transform.hpp"

namespace ECS
{
	struct TransformComponent final
	{
		Math::Transform transform;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&TransformComponent::transform);
}