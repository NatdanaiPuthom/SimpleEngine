#pragma once
#include "Engine/Math/Vector3.hpp"

namespace SimpleUtilities
{
	class Ray final
	{
	public:
		Ray();
		Ray(const Ray& aRay);
		Ray(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection);

		void InitWith2Points(const Math::Vector3f& aOrigin, const Math::Vector3f& aPoint);
		void InitWithOriginAndDirection(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection);
		Math::Vector3f GetInverseDirection() const;
		Math::Vector3f GetOrigin() const;
		Math::Vector3f GetDirection() const;
	private:
		Math::Vector3f myOrigin;
		Math::Vector3f myDirection;
	};

	inline Ray::Ray() : myOrigin(Math::Vector3f()), myDirection(Math::Vector3f())
	{
	}

	inline Ray::Ray(const Ray& aRay) : myOrigin(aRay.myOrigin), myDirection(aRay.myDirection)
	{
	}

	inline Ray::Ray(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection) : myOrigin(aOrigin), myDirection(aDirection)
	{
	}

	inline void Ray::InitWith2Points(const Math::Vector3f& aOrigin, const Math::Vector3f& aPoint)
	{
		myDirection = aOrigin - aPoint;
		myOrigin = aOrigin;
	}

	inline void Ray::InitWithOriginAndDirection(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;
	}

	inline Math::Vector3f Ray::GetOrigin() const
	{
		return myOrigin;
	}

	inline Math::Vector3f Ray::GetDirection() const
	{
		return myDirection;
	}

	inline Math::Vector3f Ray::GetInverseDirection() const
	{
		return Math::Vector3f(1.0f / myDirection.x, 1.0f / myDirection.y, 1.0f / myDirection.z);
	}
}