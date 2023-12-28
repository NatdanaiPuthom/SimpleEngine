#pragma once
#include "Engine/SimpleUtilities/Vector3.hpp"

namespace SimpleUtilities
{
	class Ray final
	{
	public:
		Ray();
		Ray(const Ray& aRay);
		Ray(const Vector3f& aOrigin, const Vector3f& aDirection);

		void InitWith2Points(const Vector3f& aOrigin, const Vector3f& aPoint);
		void InitWithOriginAndDirection(const Vector3f& aOrigin, const Vector3f& aDirection);
		Vector3f GetInverseDirection() const;
		Vector3f GetOrigin() const;
		Vector3f GetDirection() const;
	private:
		Vector3f myOrigin;
		Vector3f myDirection;
	};

	inline Ray::Ray() : myOrigin(Vector3f()), myDirection(Vector3f())
	{
	}

	inline Ray::Ray(const Ray& aRay) : myOrigin(aRay.myOrigin), myDirection(aRay.myDirection)
	{
	}

	inline Ray::Ray(const Vector3f& aOrigin, const Vector3f& aDirection) : myOrigin(aOrigin), myDirection(aDirection)
	{
	}

	inline void Ray::InitWith2Points(const Vector3f& aOrigin, const Vector3f& aPoint)
	{
		myDirection = aOrigin - aPoint;
		myOrigin = aOrigin;
	}

	inline void Ray::InitWithOriginAndDirection(const Vector3f& aOrigin, const Vector3f& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;
	}

	inline Vector3f Ray::GetOrigin() const
	{
		return myOrigin;
	}

	inline Vector3f Ray::GetDirection() const
	{
		return myDirection;
	}

	inline Vector3f Ray::GetInverseDirection() const
	{
		return Vector3f(1.0f / myDirection.x, 1.0f / myDirection.y, 1.0f / myDirection.z);
	}
}
