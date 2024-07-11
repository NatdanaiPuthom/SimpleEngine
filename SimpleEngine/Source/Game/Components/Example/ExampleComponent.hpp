#pragma once

/* NOTE (v11.2.5)
*
* REGISTER_AND_EXPOSE_PROPERTY() => void __RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const bool aShouldExpose = true, const bool aCanEdit = true) {}
*
* 1. Go to Game/Components/AddComponentHere/AllGameComponents.hpp
* 2. Include this file:	#include "Game/Components/Example/ExampleComponent.hpp"
* 
*/

namespace ECS
{
	struct ExampleComponent final
	{
		bool isExample = true;

		bool youCanSeeMeAndTouchMe = true;
		bool isVisible = false;

		std::string youCanSeeMeButCannotTouch = "Aaaaaa";
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(ExampleComponent);

	REGISTER_AND_EXPOSE_PROPERTY(&ExampleComponent::isExample);

	REGISTER_AND_EXPOSE_PROPERTY(&ExampleComponent::youCanSeeMeAndTouchMe, true);
	REGISTER_AND_EXPOSE_PROPERTY(&ExampleComponent::isVisible, false);

	REGISTER_AND_EXPOSE_PROPERTY(&ExampleComponent::youCanSeeMeButCannotTouch, true, false);
}