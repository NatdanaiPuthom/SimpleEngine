#pragma once

namespace ECS
{
	struct NullComponent final
	{
		bool iAmYourSolution = true;
		bool dontTouchMe = true;
	};
}

#include "Engine/ECS/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(NullComponent);
	EXPOSE_VARIABLE(&NullComponent::iAmYourSolution);
	EXPOSE_VARIABLE(&NullComponent::dontTouchMe);
}