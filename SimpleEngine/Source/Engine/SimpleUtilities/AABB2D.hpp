#pragma once
#include "Engine/Math/Vector2.hpp"

namespace SimpleUtilities
{
	class AABB2D
	{
	public:
		AABB2D();
		AABB2D(const AABB2D& aAABB2D);
		AABB2D(const Math::Vector2f& aMin, const Math::Vector2f& aMax);

		void InitWithMinAndMax(const Math::Vector2f& aMin, const Math::Vector2f& aMax);
		bool IsInside(const Math::Vector2f& aPosition) const;
	public:
		Math::Vector2f GetMax() const;
		Math::Vector2f GetMin() const;
	private:
		Math::Vector2f myMaxPoint;
		Math::Vector2f myMinPoint;
	};

	inline AABB2D::AABB2D() : myMaxPoint(Math::Vector2f()), myMinPoint(Math::Vector2f())
	{
	}

	inline AABB2D::AABB2D(const AABB2D& aAABB2D) : myMaxPoint(aAABB2D.myMaxPoint), myMinPoint(aAABB2D.myMinPoint)
	{
	}

	inline AABB2D::AABB2D(const Math::Vector2f& aMin, const Math::Vector2f& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	}

	inline void AABB2D::InitWithMinAndMax(const Math::Vector2f& aMin, const Math::Vector2f& aMax)
	{
		myMaxPoint = aMax;
		myMinPoint = aMin;
	}

	inline bool AABB2D::IsInside(const Math::Vector2f& aPosition) const
	{
		return (
			aPosition.x >= myMinPoint.x && aPosition.x <= myMaxPoint.x &&
			aPosition.y >= myMinPoint.y && aPosition.y <= myMaxPoint.y
			);
	}

	inline Math::Vector2f AABB2D::GetMax() const
	{
		return myMaxPoint;
	}

	inline Math::Vector2f AABB2D::GetMin() const
	{
		return myMinPoint;
	}
}