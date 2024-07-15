#pragma once
#include <string>
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	struct RealEnemyComponent final
	{
		bool isExample = true;

		bool youCanSeeMeAndTouchMe = true;
		bool isVisible = false;

		std::string youCanSeeMeButCannotTouch = "Aaaaaa";
	};

	REGISTER_COMPONENT(RealEnemyComponent);

	REGISTER_AND_EXPOSE_PROPERTY(&RealEnemyComponent::isExample);

	REGISTER_AND_EXPOSE_PROPERTY(&RealEnemyComponent::youCanSeeMeAndTouchMe, true);
	REGISTER_AND_EXPOSE_PROPERTY(&RealEnemyComponent::isVisible, false);

	REGISTER_AND_EXPOSE_PROPERTY(&RealEnemyComponent::youCanSeeMeButCannotTouch, true, true);
}