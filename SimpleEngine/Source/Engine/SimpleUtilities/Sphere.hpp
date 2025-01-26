#pragma once
#include "Engine/Math/Vector3.hpp"

namespace Simple
{

	class Sphere final
	{
	public:


		constexpr Sphere() = default;

		constexpr Sphere(Math::Vector3f aCenter, float aRadius)
			: myCenter(aCenter)
			, myRadius(aRadius)
		{
		}

		void SetCenter(Math::Vector3f aCenter)
		{
			myCenter = aCenter;
		}

		void SetRadius(float aRadius)
		{
			myRadius = aRadius;
		}

		[[nodiscard]] constexpr Math::Vector3f GetCenter() const
		{
			return myCenter;
		}

		[[nodiscard]] constexpr float GetRadius() const
		{
			return myRadius;
		}

	public:

		Math::Vector3f myCenter;
		float myRadius = 0.f;
	};

}