#include "Engine/Precomplied/EnginePch.hpp"
#include "Collider.hpp"
#include "CollisionFunctions.hpp"
#include "SimpleUtilities/DebugShapes.hpp"

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

	bool Collider::DoesCollideWith(const Shape& aOtherShape) const
	{
		return std::visit([](const auto& aShape1, const auto& aShape2) -> bool
			{
				return DetectCollision(aShape1, aShape2);
			}, myShape, aOtherShape);
	}

	void Collider::Render() const
	{
		std::visit(ShapeRenderVisitor{ myColor }, myShape);
	}
}