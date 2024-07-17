#pragma once
#include <vector>
#include "Engine/Math/Vector3.hpp"
namespace ECS
{

	struct  RewindTestComponent final
	{

		bool goToFirstPoint = true;

		Math::Vector3f wayPoint1;
		Math::Vector3f wayPoint2;
		Math::Vector3f addRotation{ 0,1,0 };
	};
}



#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(RewindTestComponent);

	REGISTER_AND_EXPOSE_PROPERTY(&RewindTestComponent::wayPoint1,true,true);
	REGISTER_AND_EXPOSE_PROPERTY(&RewindTestComponent::wayPoint2,true,true);

}