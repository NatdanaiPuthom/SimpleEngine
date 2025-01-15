#pragma once
#include "SimpleUtilities/Ray.hpp"
#include "SimpleUtilities/AABB3D.hpp"
#include "SimpleUtilities/Sphere.hpp"
#include "Math/Math.hpp"

#undef min

namespace Simple
{

	constexpr bool DetectCollision(const Sphere& aSphere1, const Sphere& aSphere2)
	{
		const Math::Vector3f centerVector = aSphere1.GetCenter() - aSphere2.GetCenter();
		const float distanceSquared = centerVector.Dot(centerVector);
		const float sumRadiiSquared = (aSphere1.GetRadius() + aSphere2.GetRadius()) * (aSphere1.GetRadius() + aSphere2.GetRadius());
		return distanceSquared <= sumRadiiSquared;
	}

	constexpr bool DetectCollision(const Sphere& aSphere, const AABB3D& aAABB)
	{
		const Math::Vector3f sphereCenter = aSphere.GetCenter();
		// Find the closest point to the sphere within the AABB
		Math::Vector3f closestPoint
		{
			std::max(aAABB.GetMin().x, std::min(sphereCenter.x, aAABB.GetMax().x)),
			std::max(aAABB.GetMin().y, std::min(sphereCenter.y, aAABB.GetMax().y)),
			std::max(aAABB.GetMin().z, std::min(sphereCenter.z, aAABB.GetMax().z))
		};

		// Calculate the distance between the closest point and the sphere's center
		const float distanceSquared = (closestPoint.x - sphereCenter.x) * (closestPoint.x - sphereCenter.x) +
			(closestPoint.y - sphereCenter.y) * (closestPoint.y - sphereCenter.y) +
			(closestPoint.z - sphereCenter.z) * (closestPoint.z - sphereCenter.z);

		// If the distance is less than the radius, they intersect
		return distanceSquared < (aSphere.GetRadius() * aSphere.GetRadius());
	}

	constexpr bool DetectCollision(const Sphere& aSphere, const Ray& aRay)
	{
		const Math::Vector3f rayOriginToSphere = aSphere.GetCenter() - aRay.GetOrigin();

		const float sphereRadiusSqr = aSphere.GetRadius() * aSphere.GetRadius();
		if (rayOriginToSphere.LengthSqr() <= sphereRadiusSqr)
		{
			return true;
		}

		const float projection = rayOriginToSphere.Dot(aRay.GetDirection());

		if (projection < 0)
		{
			return false;
		}

		const Math::Vector3f closestPoint = aRay.GetOrigin() + projection * aRay.GetDirection();

		const float distanceSqr = (closestPoint - aSphere.GetCenter()).LengthSqr();

		return distanceSqr <= sphereRadiusSqr;
	}

	constexpr bool DetectCollision(const AABB3D& aAABB1, const AABB3D& aAABB2)
	{
		// Check for overlap along the x-axis
		if (aAABB1.GetMax().x < aAABB2.GetMin().x || aAABB1.GetMin().x > aAABB2.GetMax().x)
		{
			return false; // No overlap
		}

		// Check for overlap along the y-axis
		if (aAABB1.GetMax().y < aAABB2.GetMin().y || aAABB1.GetMin().y > aAABB2.GetMax().y)
		{
			return false; // No overlap
		}

		// Check for overlap along the z-axis
		if (aAABB1.GetMax().z < aAABB2.GetMin().z || aAABB1.GetMin().z > aAABB2.GetMax().z)
		{
			return false; // No overlap
		}

		// If there is no gap along any axis, the AABBs overlap
		return true;
	}

	constexpr bool DetectCollision(const AABB3D& aAABB, const Sphere& aSphere)
	{
		return DetectCollision(aSphere, aAABB);
	}

	constexpr bool DetectCollision(const AABB3D& aAABB, const Ray& aRay)
	{
		const Math::Vector3f invDir = 1.0f / aRay.GetDirection();

		float tMin = (aAABB.GetMin().x - aRay.GetOrigin().x) * invDir.x;
		float tMax = (aAABB.GetMax().x - aRay.GetOrigin().x) * invDir.x;

		if (tMin > tMax) std::swap(tMin, tMax);

		float tYMin = (aAABB.GetMin().y - aRay.GetOrigin().y) * invDir.y;
		float tYMax = (aAABB.GetMax().y - aRay.GetOrigin().y) * invDir.y;

		if (tYMin > tYMax) std::swap(tYMin, tYMax);

		if ((tMin > tYMax) || (tYMin > tMax)) return false;

		if (tYMin > tMin) tMin = tYMin;
		if (tYMax < tMax) tMax = tYMax;

		float tZMin = (aAABB.GetMin().z - aRay.GetOrigin().z) * invDir.z;
		float tZMax = (aAABB.GetMax().z - aRay.GetOrigin().z) * invDir.z;

		if (tZMin > tZMax) std::swap(tZMin, tZMax);

		if ((tMin > tZMax) || (tZMin > tMax)) return false;

		return true;
	}

	constexpr bool DetectCollision(const Ray& aRay, const Sphere& aSphere)
	{
		return DetectCollision(aSphere, aRay);
	}

	constexpr bool DetectCollision(const Ray& aRay, const AABB3D& aAABB)
	{
		return DetectCollision(aAABB, aRay);
	}

	constexpr bool DetectCollision(const Ray& aRay1, const Ray& aRay2)
	{
		constexpr float TOLERANCE = 1e-6f;

		const Math::Vector3f origin1 = aRay1.GetOrigin();
		const Math::Vector3f origin2 = aRay2.GetOrigin();
		const Math::Vector3f dir1 = aRay1.GetDirection();
		const Math::Vector3f dir2 = aRay2.GetDirection();

		const Math::Vector3f r = origin2 - origin1;
		const float a = Math::Dot(dir1, dir1);
		const float b = Math::Dot(dir1, dir2);
		const float c = Math::Dot(dir2, dir2);
		const float d = Math::Dot(dir1, r);
		const float e = Math::Dot(dir2, r);

		const float denominator = a * c - b * b;

		// Check for parallelism
		if (Math::Abs(denominator) < TOLERANCE)
		{

			const Math::Vector3f rayToPoint = aRay1.GetOrigin() - aRay2.GetOrigin();

			if (Math::Abs(aRay2.GetDirection().x * rayToPoint.y - aRay2.GetDirection().y * rayToPoint.x) < 1e-6 &&
				Math::Abs(aRay2.GetDirection().y * rayToPoint.z - aRay2.GetDirection().z * rayToPoint.y) < 1e-6 &&
				Math::Abs(aRay2.GetDirection().z * rayToPoint.x - aRay2.GetDirection().x * rayToPoint.z) < 1e-6)
			{

				const float dotProduct = aRay2.GetDirection().Dot(rayToPoint);

				if (dotProduct >= 0)
				{
					return true;
				}
			}

			return false;
		}

		const float t1 = (d * c - b * e) / denominator;
		const float t2 = (e * a - b * d) / denominator;

		const Math::Vector3f point1 = origin1 + t1 * dir1;
		const Math::Vector3f point2 = origin2 + t2 * dir2;

		return Math::Distance(point1, point2) < TOLERANCE;

		//if (Math::Distance(point1, point2) < TOLERANCE)
		//{
		//	return true; // Rays intersect at point1 (or point2)
		//}
		//else
		//{
		//	return false; // Rays do not intersect
		//}
	}



}