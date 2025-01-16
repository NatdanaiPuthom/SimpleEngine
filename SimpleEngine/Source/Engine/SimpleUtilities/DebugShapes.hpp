#pragma once
#include "Sphere.hpp"
#include "Ray.hpp"
#include "AABB3D.hpp"
#include "Color.hpp"

namespace Simple
{
	void RenderSphere(const Sphere& aSphere, const Color& aColor);
	void RenderAABB3D(const AABB3D& aAABB3D, const Color& aColor);
	void RenderRay(const Ray& aRay, const Color& aColor);
}