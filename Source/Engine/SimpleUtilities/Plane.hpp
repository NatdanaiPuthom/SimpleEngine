#pragma once
#include "Engine/SimpleUtilities/Vector3.hpp"

namespace SimpleUtilities
{
	class Plane final
	{
	public:
		Plane();
		Plane(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2);
		Plane(const Vector3f& aPoint0, const Vector3f& aNormal);

		void InitWith3Points(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2);
		void InitWithPointAndNormal(const Vector3f& aPoint, const Vector3f& aNormal);

		bool IsInside(const Vector3f& aPosition) const;

		const Vector3f& GetNormal() const;
		Vector3f GetPosition() const;
	private:
		Vector3f myPosition;
		Vector3f myNormal;
	};

	inline Plane::Plane() : myNormal(Vector3f(0,0,0)), myPosition({0,0,0})
	{

	}

	inline Plane::Plane(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2)
	{
		InitWith3Points(aPoint0, aPoint1, aPoint2);
	}

	inline Plane::Plane(const Vector3f& aPoint0, const Vector3f& aNormal)
	{
		InitWithPointAndNormal(aPoint0, aNormal);
	}

	inline void Plane::InitWith3Points(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2)
	{
		Vector3f vector1 = aPoint1 - aPoint0;
		Vector3f vector2 = aPoint2 - aPoint0;

		myNormal = SimpleUtilities::Cross(vector1, vector2);
		myNormal.Normalize();

		myPosition = aPoint0;
	}

	inline void Plane::InitWithPointAndNormal(const Vector3f& aPoint, const Vector3f& aNormal)
	{
		myNormal = aNormal;
		myPosition = aPoint;
	}

	inline bool Plane::IsInside(const Vector3f& aPosition) const
	{
		const float pa = SimpleUtilities::Dot(aPosition - myPosition, GetNormal());
		return pa <= 0.0f;
	}

	inline const Vector3f& Plane::GetNormal() const
	{
		return myNormal;
	}

	inline Vector3f Plane::GetPosition() const
	{
		return myPosition;
	}
}