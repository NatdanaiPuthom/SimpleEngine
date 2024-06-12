#pragma once

namespace ECS
{
	struct NullComponent final
	{
		bool iAmYourSolution = true;
		bool dontTouchMe = true;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(NullComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::iAmYourSolution);
	REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::dontTouchMe);
}