#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	struct NullComponent final
	{
		float test = 0.0f;
		bool iAmYourSolution = true;
		bool dontTouchMe = true;
	};
}

namespace ECS
{
	REGISTER_COMPONENT(NullComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::iAmYourSolution);
	REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::dontTouchMe);
	REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::test);
}