#include "Engine/Precomplied/EnginePch.hpp"
#include "DebugShapes.hpp"
#include "engine.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "Bounds.hpp"
#include "Collision/CollisionFunctions.hpp"

namespace Simple
{

	void RenderSphere(const Sphere& aSphere, const Color& aColor)
	{
		Drawer::Sphere s;
		s.color = aColor;
		s.position = aSphere.GetCenter();
		s.radius = aSphere.GetRadius();
		Global::GetRenderer()->Push(s);
	}

	void RenderRay(const Ray& aRay, const Color& aColor)
	{
		Drawer::Line line
		{ 
			.color = aColor,
			.startPosition = aRay.GetOrigin(), 
			.endPosition = aRay.GetOrigin() + aRay.GetDirection() * 10000.f
		};
		Global::GetRenderer()->Push(line);
	}

	void RenderAABB3D(const AABB3D& aAABB3D, const Color& aColor)
	{
		Global::GetRenderer()->Push(Drawer::BoundingBox3DData(Math::Matrix4x4f{}, BoundingBox3D{ .min = aAABB3D.GetMin(), .max = aAABB3D.GetMax() }, aColor));
	}

}
