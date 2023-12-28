#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "SimpleUtilities/AABB.hpp"
#include "SimpleUtilities/Plane.hpp"
#include "SimpleUtilities/Ray.hpp"

namespace SimpleUtilities
{
	static bool IntersectionPlaneRay(const Plane& aPlane, const Ray& aRay, Vector3f& aOutIntersectionPoint)
	{
		const float epsilon = 0.0001f;

		float maxDistance = FLT_MAX;
		Vector3f noIntersection = aRay.GetOrigin() + aRay.GetDirection() * maxDistance; //There's no intersection so we set value to infinite distance along the ray's direction

		float projectionOnPlaneNormal = SimpleUtilities::Dot(aPlane.GetNormal(), (aRay.GetOrigin() - aPlane.GetPosition()));

		if (std::abs(projectionOnPlaneNormal) < epsilon)
		{
			aOutIntersectionPoint = aRay.GetOrigin();
			return true;
		}

		float denom = SimpleUtilities::Dot(aRay.GetDirection(), aPlane.GetNormal());

		if (std::abs(denom) < epsilon)
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

	static inline bool IntersectionAABBRay(const AABB3D& aAABB, const Ray& aRay, Vector3f& aOutIntersectionPoint)
	{
		if (aAABB.IsInside(aRay.GetOrigin()))
		{
			aOutIntersectionPoint = aRay.GetOrigin();
			return true;
		}

		const Plane planes[6] =
		{
			Plane(aAABB.GetMin(), Vector3f(-1, 0, 0)),
			Plane(aAABB.GetMin(), Vector3f(0, -1, 0)),
			Plane(aAABB.GetMin(), Vector3f(0, 0, -1)),
			Plane(aAABB.GetMax(), Vector3f(1, 0, 0)),
			Plane(aAABB.GetMax(), Vector3f(0, 1, 0)),
			Plane(aAABB.GetMax(), Vector3f(0, 0, 1))
		};

		float intersectionDistance = FLT_MAX;
		Vector3f closestIntersectionPoint;

		for (int i = 0; i < 6; i++)
		{
			if (IntersectionPlaneRay(planes[i], aRay, aOutIntersectionPoint))
			{
				if (aAABB.IsInside(aOutIntersectionPoint))
				{
					const Vector3f distance = aOutIntersectionPoint - aRay.GetOrigin();

					if (distance.Length() < intersectionDistance)
					{
						intersectionDistance = static_cast<float>(distance.Length());
						closestIntersectionPoint = aOutIntersectionPoint;
					}
				}
			}
		}

		if (intersectionDistance == FLT_MAX)
		{
			return false;
		}

		aOutIntersectionPoint = closestIntersectionPoint;
		return true;
	}
}