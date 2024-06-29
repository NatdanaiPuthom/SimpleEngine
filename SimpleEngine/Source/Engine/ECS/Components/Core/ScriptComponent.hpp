#pragma once
#include "NodeScript/SimpleScript/Core/Instance/ScriptInstance.h"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace SCRIPT
{
	REGISTER_DATATYPE_POINTER(ScriptInstance);
}

namespace ECS
{
	
	struct ScriptComponent final
	{
		SCRIPT::ScriptInstance* scriptInstance;
	};
}


namespace ECS
{
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::scriptInstance);
}