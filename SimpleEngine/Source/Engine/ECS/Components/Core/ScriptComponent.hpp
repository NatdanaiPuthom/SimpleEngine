#pragma once
#include "NodeScript/SimpleScript/Core/Instance/FlyClassInstance.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace Fly
{
	REGISTER_DATATYPE_POINTER(ClassInstance);
}

namespace ECS
{
	
	struct ScriptComponent final
	{
		Fly::ClassInstance* classInstance;
	};
}


namespace ECS
{
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::classInstance);
}