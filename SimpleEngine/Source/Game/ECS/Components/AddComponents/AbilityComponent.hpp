#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	COMPONENT(AbilitComponent) final
	{
		std::string name;
		int manaCost;
		int key;
		void (*execute)(void);
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&AbilitComponent::name);
	REGISTER_AND_EXPOSE_PROPERTY(&AbilitComponent::manaCost);
}