#pragma once
#include "NodeScript/SimpleScript/Core/View/FlyClassInstanceView.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace Fly
{
	REGISTER_DATATYPE(ClassInstanceView);
}

namespace ECS
{
	
	struct ScriptComponent final
	{
		Fly::ClassInstanceView classInstanceView;
	};
}


namespace ECS
{
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&ScriptComponent::classInstanceView);
}