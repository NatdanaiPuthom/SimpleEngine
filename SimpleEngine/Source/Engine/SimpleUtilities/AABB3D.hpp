#pragma once
#include "Engine/Math/Vector3.hpp"

namespace Simple
{
	class AABB3D
	{
	public:
		constexpr AABB3D() = default;
		constexpr AABB3D(const AABB3D& aAABB3D);
		constexpr AABB3D(const Math::Vector3f& aMin, const Math::Vector3f& aMax);

		constexpr void InitWithMinAndMax(const Math::Vector3f& aMin, const Math::Vector3f& aMax);
		constexpr bool IsInside(const Math::Vector3f& aPosition) const;

		constexpr void SetCenter(const Math::Vector3f& aCenter);
		constexpr void SetExtent(const Math::Vector3f& aBounds);
		constexpr Math::Vector3f GetExtent() const;
		constexpr Math::Vector3f GetCenter() const;
	public:
		constexpr Math::Vector3f GetMax() const;
		constexpr Math::Vector3f GetMin() const;
	private:
		Math::Vector3f myMaxPoint;
		Math::Vector3f myMinPoint;
	};

	constexpr AABB3D::AABB3D(const AABB3D& aAABB3D) : myMaxPoint(aAABB3D.myMaxPoint), myMinPoint(aAABB3D.myMinPoint)
	{
	}

	constexpr AABB3D::AABB3D(const Math::Vector3f& aMin, const Math::Vector3f& aMax)
		: myMinPoint(aMin)
		, myMaxPoint(aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	}

	constexpr void AABB3D::InitWithMinAndMax(const Math::Vector3f& aMin, const Math::Vector3f& aMax)
	{
		myMaxPoint = aMax;
		myMinPoint = aMin;
	}

	constexpr bool AABB3D::IsInside(const Math::Vector3f& aPosition) const
	{
		return (
			aPosition.x >= myMinPoint.x && aPosition.x <= myMaxPoint.x &&
			aPosition.y >= myMinPoint.y && aPosition.y <= myMaxPoint.y &&
			aPosition.z >= myMinPoint.z && aPosition.z <= myMaxPoint.z
			);
	}

	constexpr void AABB3D::SetCenter(const Math::Vector3f& aCenter)
	{
		const Math::Vector3f bounds = GetExtent();
		myMinPoint = aCenter - bounds / 2.f;
		myMaxPoint = myMinPoint + bounds;
	}

	constexpr void AABB3D::SetExtent(const Math::Vector3f& aExtent)
	{
		const Math::Vector3f min = GetExtent() - aExtent / 2.f;
		InitWithMinAndMax(min, min + aExtent);
	}

	constexpr Math::Vector3f AABB3D::GetExtent() const
	{
		return myMaxPoint - myMinPoint;
	}

	inline constexpr Math::Vector3f AABB3D::GetCenter() const
	{
		return myMinPoint + GetExtent() / 2.f;
	}

	constexpr Math::Vector3f AABB3D::GetMax() const
	{
		return myMaxPoint;
	}

	constexpr Math::Vector3f AABB3D::GetMin() const
	{
		return myMinPoint;
	}
}