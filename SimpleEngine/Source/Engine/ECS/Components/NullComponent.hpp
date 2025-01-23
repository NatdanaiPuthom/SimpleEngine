#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	COMPONENT(NullComponent) final
	{
		bool iAmYourSolution = true;
		bool dontTouchMe = true;
	};
}

namespace ECS
{
	//REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::iAmYourSolution, true, false);
	//REGISTER_AND_EXPOSE_PROPERTY(&NullComponent::dontTouchMe, true, false);
}