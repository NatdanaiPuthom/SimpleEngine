#pragma once
#include <limits>
#include "Engine/SimpleUtilities/Utility.hpp"
#include "SimpleUtilities/AABB.hpp"
#include "SimpleUtilities/Plane.hpp"
#include "SimpleUtilities/Ray.hpp"

namespace SimpleUtilities
{
	static bool IntersectionPlaneRay(const Plane& aPlane, const Ray& aRay, Vector3f& aOutIntersectionPoint)
	{
		const float dotProduct = SimpleUtilities::Dot(aPlane.GetNormal(), aRay.GetDirection());

		if (dotProduct != 0.0f)
		{
			const SimpleUtilities::Vector3f g = aPlane.GetPosition() - aRay.GetOrigin();
			const float t = SimpleUtilities::Dot(g, aPlane.GetNormal()) / dotProduct;

			if (t >= 0)
			{
				aOutIntersectionPoint = aRay.GetOrigin() + t * aRay.GetDirection();
				return true;
			}
		}

		return false;
	}

	static inline bool IntersectionAABB3DRay(const AABB3D& aAABB, const Ray& aRay, Vector3f& aOutIntersectionPoint)
	{
		const SimpleUtilities::Vector3f& rayOrigin = aRay.GetOrigin();
		const SimpleUtilities::Vector3f& rayDirection = aRay.GetDirection();

		const SimpleUtilities::Vector3f& min = aAABB.GetMin();
		const SimpleUtilities::Vector3f& max = aAABB.GetMax();

		float txMin = (min.x - rayOrigin.x) / rayDirection.x;
		float txMax = (max.x - rayOrigin.x) / rayDirection.x;

		if (txMin > txMax)
		{
			SimpleUtilities::Swap(txMin, txMax);
		}

		float tyMin = (min.y - rayOrigin.y) / rayDirection.y;
		float tyMax = (max.y - rayOrigin.y) / rayDirection.y;

		if (tyMin > tyMax)
		{
			SimpleUtilities::Swap(tyMin, tyMax);
		}

		if ((txMin > tyMax) || (tyMin > txMax))
			return false;

		if (tyMin > txMin)
		{
			txMin = tyMin;
		}

		if (tyMax < txMax)
		{
			txMax = tyMax;
		}

		float tzMin = (min.z - rayOrigin.z) / rayDirection.z;
		float tzMax = (max.z - rayOrigin.z) / rayDirection.z;

		if (tzMin > tzMax)
		{
			SimpleUtilities::Swap(tzMin, tzMax);
		}

		if ((txMin > tzMax) || (tzMin > txMax))
			return false;

		const float tMin = SimpleUtilities::GetMax(SimpleUtilities::GetMax(txMin, tyMin), tzMin);
		if (tMin < 0.0f)
			return false;

		const SimpleUtilities::Vector3f intersectionPoint = rayOrigin + tMin * rayDirection;
		aOutIntersectionPoint = intersectionPoint;

		return true;
	}
}