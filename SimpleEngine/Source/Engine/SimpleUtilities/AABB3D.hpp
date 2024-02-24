#pragma once
#include "Engine/Math/Vector3.hpp"

namespace SimpleUtilities
{
	class AABB3D
	{
	public:
		AABB3D();
		AABB3D(const AABB3D& aAABB3D);
		AABB3D(const Math::Vector3f& aMin, const Math::Vector3f& aMax);

		void InitWithMinAndMax(const Math::Vector3f& aMin, const Math::Vector3f& aMax);
		bool IsInside(const Math::Vector3f& aPosition) const;
	public:
		Math::Vector3f GetMax() const;
		Math::Vector3f GetMin() const;
	private:
		Math::Vector3f myMaxPoint;
		Math::Vector3f myMinPoint;
	};

	inline AABB3D::AABB3D() : myMaxPoint(Math::Vector3f()), myMinPoint(Math::Vector3f())
	{
	}

	inline AABB3D::AABB3D(const AABB3D& aAABB3D) : myMaxPoint(aAABB3D.myMaxPoint), myMinPoint(aAABB3D.myMinPoint)
	{
	}

	inline AABB3D::AABB3D(const Math::Vector3f& aMin, const Math::Vector3f& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	}

	inline void AABB3D::InitWithMinAndMax(const Math::Vector3f& aMin, const Math::Vector3f& aMax)
	{
		myMaxPoint = aMax;
		myMinPoint = aMin;
	}

	inline bool AABB3D::IsInside(const Math::Vector3f& aPosition) const
	{
		return (
			aPosition.x >= myMinPoint.x && aPosition.x <= myMaxPoint.x &&
			aPosition.y >= myMinPoint.y && aPosition.y <= myMaxPoint.y &&
			aPosition.z >= myMinPoint.z && aPosition.z <= myMaxPoint.z
			);
	}

	inline Math::Vector3f AABB3D::GetMax() const
	{
		return myMaxPoint;
	}

	inline Math::Vector3f AABB3D::GetMin() const
	{
		return myMinPoint;
	}
}