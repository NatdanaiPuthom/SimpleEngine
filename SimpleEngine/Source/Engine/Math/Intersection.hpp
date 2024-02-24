#pragma once
#include <limits>
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/AABB3D.hpp"
#include "Engine/SimpleUtilities/Plane.hpp"
#include "Engine/SimpleUtilities/Ray.hpp"
#include "Engine/Math/Math.hpp"

namespace Math
{
	static bool IntersectionPlaneRay(const SimpleUtilities::Plane& aPlane, const SimpleUtilities::Ray& aRay, Math::Vector3f& aOutIntersectionPoint)
	{
		const float dotProduct = Math::Dot(aPlane.GetNormal(), aRay.GetDirection());

		if (dotProduct != 0.0f)
		{
			const Math::Vector3f g = aPlane.GetPosition() - aRay.GetOrigin();
			const float t = Math::Dot(g, aPlane.GetNormal()) / dotProduct;

			if (t >= 0)
			{
				aOutIntersectionPoint = aRay.GetOrigin() + t * aRay.GetDirection();
				return true;
			}
		}

		return false;
	}

	static inline bool IntersectionAABB3DRay(const SimpleUtilities::AABB3D& aAABB, const SimpleUtilities::Ray& aRay, Math::Vector3f& aOutIntersectionPoint)
	{
		const Math::Vector3f& rayOrigin = aRay.GetOrigin();
		const Math::Vector3f& rayDirection = aRay.GetDirection();

		const Math::Vector3f& min = aAABB.GetMin();
		const Math::Vector3f& max = aAABB.GetMax();

		float txMin = (min.x - rayOrigin.x) / rayDirection.x;
		float txMax = (max.x - rayOrigin.x) / rayDirection.x;

		if (txMin > txMax)
		{
			Math::Swap(txMin, txMax);
		}

		float tyMin = (min.y - rayOrigin.y) / rayDirection.y;
		float tyMax = (max.y - rayOrigin.y) / rayDirection.y;

		if (tyMin > tyMax)
		{
			Math::Swap(tyMin, tyMax);
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
			Math::Swap(tzMin, tzMax);
		}

		if ((txMin > tzMax) || (tzMin > txMax))
			return false;

		const float tMin = Math::GetMax(Math::GetMax(txMin, tyMin), tzMin);
		if (tMin < 0.0f)
			return false;

		const Math::Vector3f intersectionPoint = rayOrigin + tMin * rayDirection;
		aOutIntersectionPoint = intersectionPoint;

		return true;
	}
}