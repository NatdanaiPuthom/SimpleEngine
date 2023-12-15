#pragma once

namespace SimpleUtilities
{
	template <typename T>
	class Vector3;

	template <typename T>
	class Plane
	{
	public:
		Plane();
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);

		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		bool IsInside(const Vector3<T>& aPosition) const;

		const Vector3<T>& GetNormal() const;
		Vector3<T> GetPosition() const;
	private:
		Vector3<T> myPosition;
		Vector3<T> myNormal;
	};

	template<typename T>
	inline Plane<T>::Plane() : myNormal(Vector3<T>(0,0,0)), myPosition({0,0,0})
	{

	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		InitWith3Points(aPoint0, aPoint1, aPoint2);
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		InitWithPointAndNormal(aPoint0, aNormal);
	}

	template<typename T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		Vector3<T> vector1 = aPoint1 - aPoint0;
		Vector3<T> vector2 = aPoint2 - aPoint0;

		myNormal = vector1.Cross(vector2);
		myNormal.Normalize();

		myPosition = aPoint0;
	}

	template<typename T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myNormal = aNormal;
		myPosition = aPoint;
	}

	template<typename T>
	inline bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		T pa = (aPosition - myPosition).Dot(GetNormal());
		return pa <= 0.0f;
	}

	template<typename T>
	inline const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;
	}

	template<typename T>
	inline Vector3<T> Plane<T>::GetPosition() const
	{
		return myPosition;
	}
}