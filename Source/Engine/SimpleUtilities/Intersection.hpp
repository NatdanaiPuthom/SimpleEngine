#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "SimpleUtilities/Vector3.hpp"
#include "SimpleUtilities/Plane.hpp"
#include "SimpleUtilities/Ray.hpp"
#include "SimpleUtilities/AABB.hpp"

namespace SimpleUtilities
{
	template <typename T>
	static bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint);

	template <typename T>
	static bool IntersectionAABBRay(const AABB3D& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint);

	template<typename T>
	inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		const T epsilon = static_cast<T>(0.0001);

		T maxDistance = FLT_MAX;
		Vector3<T> noIntersection = aRay.GetOrigin() + aRay.GetDirection() * maxDistance; //There's no intersection so we set value to infinite distance along the ray's direction

		T projectionOnPlaneNormal = SimpleUtilities::Dot(aPlane.GetNormal(), (aRay.GetOrigin()- aPlane.GetPosition()));

		if (std::abs(projectionOnPlaneNormal) < epsilon)
		{
			aOutIntersectionPoint = aRay.GetOrigin();
			return true;
		}

		T denom = SimpleUtilities::Dot(aRay.GetDirection(), aPlane.GetNormal());

		if (std::abs(denom) < epsilon)
		{
			aOutIntersectionPoint = noIntersection;
			return false;
		}

		T intersectionDistance = -projectionOnPlaneNormal / denom;

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

	template<typename T>
	inline bool IntersectionAABBRay(const AABB3D& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		if (aAABB.IsInside(aRay.GetOrigin()))
		{
			aOutIntersectionPoint = aRay.GetOrigin();
			return true;
		}

		Plane<T> planes[6] =
		{
			Plane<T>(aAABB.GetMin(), Vector3<T>(-1, 0, 0)),
			Plane<T>(aAABB.GetMin(), Vector3<T>(0, -1, 0)),
			Plane<T>(aAABB.GetMin(), Vector3<T>(0, 0, -1)),
			Plane<T>(aAABB.GetMax(), Vector3<T>(1, 0, 0)),
			Plane<T>(aAABB.GetMax(), Vector3<T>(0, 1, 0)),
			Plane<T>(aAABB.GetMax(), Vector3<T>(0, 0, 1))
		};

		float intersectionDistance = FLT_MAX;
		Vector3<T> closestIntersectionPoint;

		for (int i = 0; i < 6; i++)
		{
			if (IntersectionPlaneRay(planes[i], aRay, aOutIntersectionPoint))
			{
				if (aAABB.IsInside(aOutIntersectionPoint))
				{
					Vector3<T> distance = aOutIntersectionPoint - aRay.GetOrigin();

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