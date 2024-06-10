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
	REGISTER_PROPERTY(&NullComponent::iAmYourSolution);
	REGISTER_PROPERTY(&NullComponent::dontTouchMe);
}