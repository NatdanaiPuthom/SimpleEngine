#include "Engine/Precomplied/EnginePch.hpp"
#include "ColliderComponent.hpp"
#include "SimpleUtilities/DebugShapes.hpp"
#include "Engine/Collision/CollisionFunctions.hpp"

namespace Simple
{
	
	struct ShapeRenderVisitor
	{
		void operator()(const Sphere& aSphere) const
		{
			RenderSphere(aSphere, myColor);
		}

		void operator()(const AABB3D& aAABB) const
		{
			RenderAABB3D(aAABB, myColor);
		}

		void operator()(const Ray& aRay) const
		{
			RenderRay(aRay, myColor);
		}

		Color myColor;

	};

	bool ColliderComponentFunctions::DoesCollideWith(const ColliderComponent& aColliderComponent1, const ColliderComponent& aColliderComponent2)
	{
		return DoesCollideWith(aColliderComponent1.myShape, aColliderComponent2.myShape);
	}

	bool ColliderComponentFunctions::DoesCollideWith(const ColliderComponent& aColliderComponent, const CollisionShape& aShape)
	{
		return DoesCollideWith(aColliderComponent.myShape, aShape);
	}

	bool ColliderComponentFunctions::DoesCollideWith(const CollisionShape& aShape1, const CollisionShape& aShape2)
	{
		return std::visit([](const auto& aShape1, const auto& aShape2) -> bool
			{
				return DetectCollision(aShape1, aShape2);
			}, aShape1, aShape2);
	}

	void ColliderComponentFunctions::Render(const ColliderComponent& aColliderComponent)
	{
		std::visit(ShapeRenderVisitor{ aColliderComponent.myColor }, aColliderComponent.myShape);
	}

}