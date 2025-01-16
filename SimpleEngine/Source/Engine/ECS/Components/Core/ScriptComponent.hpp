#pragma once
#include "NodeScript/Facade/FlyClassInstanceFacade.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace Fly
{
	REGISTER_DATATYPE(ClassInstanceFacade);
}

namespace ECS
{
	COMPONENT(ScriptComponent) final
	{
		Fly::ClassInstanceFacade classInstanceFacade;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::classInstanceFacade);
}