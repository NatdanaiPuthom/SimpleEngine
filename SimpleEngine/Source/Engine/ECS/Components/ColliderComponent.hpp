#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Collision/Collider.hpp"

namespace ECS
{
	COMPONENT(ColliderComponent) final
	{
		Simple::Collider collider;
	};
}

namespace ECS
{
	//REGISTER_AND_EXPOSE_PROPERTY(&ColliderComponent::value);
}