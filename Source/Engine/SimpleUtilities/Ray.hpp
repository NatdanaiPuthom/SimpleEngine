#pragma once

namespace SimpleUtilities
{
	template<typename T>
	class Vector3;

	template <typename T>
	class Ray
	{
	public:
		Ray();
		Ray(const Ray<T>& aRay);
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);
		Vector3<T> GetInverseDirection() const;
		Vector3<T> GetOrigin() const;
		Vector3<T> GetDirection() const;
	private:
		Vector3<T> myOrigin;
		Vector3<T> myDirection;
	};

	template<typename T>
	inline Ray<T>::Ray() : myOrigin(Vector3<T>()), myDirection(Vector3<T>())
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Ray<T>& aRay) : myOrigin(aRay.myOrigin), myDirection(aRay.myDirection)
	{
	}

	template<typename T>
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection) : myOrigin(aOrigin), myDirection(aDirection)
	{
	}

	template<typename T>
	inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myDirection = aOrigin - aPoint;
		myOrigin = aOrigin;
	}

	template<typename T>
	inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;
	}

	template<typename T>
	inline Vector3<T> Ray<T>::GetOrigin() const
	{
		return myOrigin;
	}

	template<typename T>
	inline Vector3<T> Ray<T>::GetDirection() const
	{
		return myDirection;
	}

	template<typename T>
	inline Vector3<T> Ray<T>::GetInverseDirection() const
	{
		return Vector3<T>(1 / myDirection.x, 1 / myDirection.y, 1 / myDirection.z);
	}
}
