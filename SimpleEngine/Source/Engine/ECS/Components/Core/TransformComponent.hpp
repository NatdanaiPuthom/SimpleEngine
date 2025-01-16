#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Math/Transform.hpp"

namespace ECS
{
	COMPONENT(TransformComponent) final
	{
		Math::Transform transform;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&TransformComponent::transform);
}