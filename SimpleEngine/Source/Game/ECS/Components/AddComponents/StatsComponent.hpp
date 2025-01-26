#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	COMPONENT(StatsComponent) final
	{
		float currentMana = 100;
		float maxMana = 100;
		float manaPerSec = 1;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&StatsComponent::currentMana);
	REGISTER_AND_EXPOSE_PROPERTY(&StatsComponent::maxMana);
	REGISTER_AND_EXPOSE_PROPERTY(&StatsComponent::manaPerSec);
}