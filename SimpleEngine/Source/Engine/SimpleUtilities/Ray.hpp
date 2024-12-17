#pragma once
#include "Engine/Math/Vector3.hpp"

namespace SimpleUtilities
{
	class Ray final
	{
	public:
		constexpr Ray();
		constexpr Ray(const Ray& aRay);
		constexpr Ray(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection);

		constexpr void InitWith2Points(const Math::Vector3f& aOrigin, const Math::Vector3f& aPoint);
		constexpr void InitWithOriginAndDirection(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection);
		constexpr Math::Vector3f GetInverseDirection() const;
		constexpr Math::Vector3f GetOrigin() const;
		constexpr Math::Vector3f GetDirection() const;

		constexpr void SetOrigin(const Math::Vector3f& aOrigin);
		constexpr void SetDirection(const Math::Vector3f& aDirection);
	private:
		Math::Vector3f myOrigin;
		Math::Vector3f myDirection;
	};

	constexpr Ray::Ray() : myDirection(Math::ForwardVector<float>)
	{
	}

	constexpr Ray::Ray(const Ray& aRay) : myOrigin(aRay.myOrigin), myDirection(aRay.myDirection)
	{
	}

	constexpr Ray::Ray(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection) : myOrigin(aOrigin), myDirection(aDirection)
	{
	}

	constexpr void Ray::InitWith2Points(const Math::Vector3f& aOrigin, const Math::Vector3f& aPoint)
	{
		myDirection = aOrigin - aPoint;
		myOrigin = aOrigin;
	}

	constexpr void Ray::InitWithOriginAndDirection(const Math::Vector3f& aOrigin, const Math::Vector3f& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;
	}

	constexpr Math::Vector3f Ray::GetOrigin() const
	{
		return myOrigin;
	}

	constexpr Math::Vector3f Ray::GetDirection() const
	{
		return myDirection;
	}

	constexpr void Ray::SetOrigin(const Math::Vector3f& aOrigin)
	{
		myOrigin = aOrigin;
	}

	constexpr void Ray::SetDirection(const Math::Vector3f& aDirection)
	{
		myDirection = aDirection;
	}

	constexpr Math::Vector3f Ray::GetInverseDirection() const
	{
		return Math::Vector3f(1.0f / myDirection.x, 1.0f / myDirection.y, 1.0f / myDirection.z);
	}
}