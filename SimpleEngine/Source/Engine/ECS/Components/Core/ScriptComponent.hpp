#pragma once
#include "NodeScript/Proxy/FlyClassInstanceProxy.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace Fly
{
	REGISTER_DATATYPE(ClassInstanceProxy);
}

namespace ECS
{
	COMPONENT(ScriptComponent) final
	{
		Fly::ClassInstanceProxy classInstance;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::classInstance);
}