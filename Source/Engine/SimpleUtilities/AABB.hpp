#pragma once
#include "Engine/SimpleUtilities/Vector3.hpp"

namespace SimpleUtilities
{
	class AABB3D
	{
	public:
		AABB3D();
		AABB3D(const AABB3D& aAABB3D);
		AABB3D(const Vector3f& aMin, const Vector3f& aMax);

		void InitWithMinAndMax(const Vector3f& aMin, const Vector3f& aMax);
		bool IsInside(const Vector3f& aPosition) const;
	public:
		Vector3f GetMax() const;
		Vector3f GetMin() const;
	private:
		Vector3f myMaxPoint;
		Vector3f myMinPoint;
	};

	inline AABB3D::AABB3D() : myMaxPoint(Vector3f()), myMinPoint(Vector3f())
	{
	}

	inline AABB3D::AABB3D(const AABB3D& aAABB3D) : myMaxPoint(aAABB3D.myMaxPoint), myMinPoint(aAABB3D.myMinPoint)
	{
	}

	inline AABB3D::AABB3D(const Vector3f& aMin, const Vector3f& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	}

	inline void AABB3D::InitWithMinAndMax(const Vector3f& aMin, const Vector3f& aMax)
	{
		myMaxPoint = aMax;
		myMinPoint = aMin;
	}

	inline bool AABB3D::IsInside(const Vector3f& aPosition) const
	{
		return (
			aPosition.x >= myMinPoint.x && aPosition.x <= myMaxPoint.x &&
			aPosition.y >= myMinPoint.y && aPosition.y <= myMaxPoint.y &&
			aPosition.z >= myMinPoint.z && aPosition.z <= myMaxPoint.z
			);
	}

	inline Vector3f AABB3D::GetMax() const
	{
		return myMaxPoint;
	}

	inline Vector3f AABB3D::GetMin() const
	{
		return myMinPoint;
	}
}