#pragma once
#include "NodeScript/Facade/FlyClassInstanceFacade.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace Fly
{
	REGISTER_DATATYPE(ClassInstanceFacade);
}

namespace ECS
{
	
	struct ScriptComponent final
	{
		Fly::ClassInstanceFacade classInstanceFacade;
	};
}


namespace ECS
{
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::classInstanceFacade);
}