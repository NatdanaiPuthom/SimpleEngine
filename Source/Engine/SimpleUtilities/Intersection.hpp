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
		constexpr float epsilonValue = std::numeric_limits<float>::epsilon();;

		float maxDistance = FLT_MAX;
		Vector3f noIntersection = aRay.GetOrigin() + aRay.GetDirection() * maxDistance; //There's no intersection so we set value to infinite distance along the ray's direction

		float projectionOnPlaneNormal = SimpleUtilities::Dot(aPlane.GetNormal(), (aRay.GetOrigin() - aPlane.GetPosition()));

		if (std::abs(projectionOnPlaneNormal) < epsilonValue)
		{
			aOutIntersectionPoint = aRay.GetOrigin();
			return true;
		}

		float denom = SimpleUtilities::Dot(aRay.GetDirection(), aPlane.GetNormal());

		if (std::abs(denom) < epsilonValue)
		{
			aOutIntersectionPoint = noIntersection;
			return false;
		}

		float intersectionDistance = -projectionOnPlaneNormal / denom;

		if (intersectionDistance < 0)
		{
			aOutIntersectionPoint = noIntersection;
			return false;
		}
		else
		{
			aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * intersectionDistance;
			return true;
		}
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