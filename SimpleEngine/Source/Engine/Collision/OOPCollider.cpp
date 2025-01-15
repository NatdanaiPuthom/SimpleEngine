#include "Engine/Precomplied/EnginePch.hpp"
#include "OOPCollider.hpp"
#include "CollisionFunctions.hpp"

namespace Simple
{

	bool OOP_Collider::DoesCollideWith(const OOP_Collider& aOtherCollider) const
	{
		return myCollisionShape->TryCollision(*aOtherCollider.myCollisionShape);
	}

}