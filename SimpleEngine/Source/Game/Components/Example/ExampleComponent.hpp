#pragma once

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

/* NOTE (v11.2.5)
* 
* //__RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const bool aShouldExpose = true, const bool aCanEdit = true)
* 
* Go to Game/Components/IncludeMe/AllGameComponents
* 
* Add:	#include "Game/Components/Example/ExampleComponent.hpp
*/