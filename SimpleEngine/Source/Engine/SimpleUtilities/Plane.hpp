#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Math.hpp"

namespace SimpleUtilities
{
	class Plane final
	{
	public:
		Plane();
		Plane(const Math::Vector3f& aPoint0, const Math::Vector3f& aPoint1, const Math::Vector3f& aPoint2);
		Plane(const Math::Vector3f& aPoint0, const Math::Vector3f& aNormal);

		void InitWith3Points(const Math::Vector3f& aPoint0, const Math::Vector3f& aPoint1, const Math::Vector3f& aPoint2);
		void InitWithPointAndNormal(const Math::Vector3f& aPoint, const Math::Vector3f& aNormal);

		bool IsInside(const Math::Vector3f& aPosition) const;

		const Math::Vector3f& GetNormal() const;
		Math::Vector3f GetPosition() const;
	private:
		Math::Vector3f myPosition;
		Math::Vector3f myNormal;
	};

	inline Plane::Plane() : myNormal(Math::Vector3f(0,0,0)), myPosition({0,0,0})
	{

	}

	inline Plane::Plane(const Math::Vector3f& aPoint0, const Math::Vector3f& aPoint1, const Math::Vector3f& aPoint2)
	{
		InitWith3Points(aPoint0, aPoint1, aPoint2);
	}

	inline Plane::Plane(const Math::Vector3f& aPoint0, const Math::Vector3f& aNormal)
	{
		InitWithPointAndNormal(aPoint0, aNormal);
	}

	inline void Plane::InitWith3Points(const Math::Vector3f& aPoint0, const Math::Vector3f& aPoint1, const Math::Vector3f& aPoint2)
	{
		Math::Vector3f vector1 = aPoint1 - aPoint0;
		Math::Vector3f vector2 = aPoint2 - aPoint0;

		myNormal = Math::Cross(vector1, vector2);
		myNormal.Normalize();

		myPosition = aPoint0;
	}

	inline void Plane::InitWithPointAndNormal(const Math::Vector3f& aPoint, const Math::Vector3f& aNormal)
	{
		myNormal = aNormal;
		myPosition = aPoint;
	}

	inline bool Plane::IsInside(const Math::Vector3f& aPosition) const
	{
		const float pa = Math::Dot(aPosition - myPosition, GetNormal());
		return pa <= 0.0f;
	}

	inline const Math::Vector3f& Plane::GetNormal() const
	{
		return myNormal;
	}

	inline Math::Vector3f Plane::GetPosition() const
	{
		return myPosition;
	}
}