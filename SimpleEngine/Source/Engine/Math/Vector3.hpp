#pragma once
#include <iostream>
#include "Engine/Math/Vector2.hpp"

namespace Math
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		Vector3<T>();
		~Vector3<T>() = default;

		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;

		Vector3<T>& operator=(const Vector3<T>& aVector) = default;

		Vector2<T> AsVector2() const;
		Vector2<T> AsVector2XZ() const;

		Vector3<T> GetNormalized() const;

		T LengthSqr() const;
		T Length() const;

		void Normalize();
	};

	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;
	typedef Vector3<unsigned int> Vector3ui;

	template <class T> Vector3<T> operator+(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB);
	template <class T> Vector3<T> operator-(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB);

	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar);
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector);
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1);

	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar);
	template <class T> Vector3<T> operator/(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB);

	template <class T> void operator+=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB);
	template <class T> void operator-=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB);
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar);

	template <class T> std::ostream& operator<<(std::ostream& aOS, const Vector3<T>& aVector);

	template<class T>
	inline Vector3<T>::Vector3() : Vector3(0, 0, 0)
	{
	}

	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) : x(aX), y(aY), z(aZ)
	{
	}

	template<class T>
	inline Vector2<T> Vector3<T>::AsVector2() const
	{
		return Vector2<T>(x, y);
	}

	template<class T>
	inline Vector2<T> Vector3<T>::AsVector2XZ() const
	{
		return Vector2<T>(x, z);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		const T magnitude = (x * x) + (y * y) + (z * z);

		if (magnitude == 0 || magnitude == 1)
		{
			return *this;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));

		return Vector3<T>(x * normalize, y * normalize, z * normalize);
	}

	template<class T>
	inline void Vector3<T>::Normalize()
	{
		const T magnitude = (x * x) + (y * y) + (z * z);

		if (magnitude == 0 || magnitude == 1)
		{
			return;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));
		x *= normalize;
		y *= normalize;
		z *= normalize;
	}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return abs((x * x) + (y * y) + (z * z));
	}

	template<class T>
	inline T Vector3<T>::Length() const
	{
		return static_cast<T> (sqrt((x * x) + (y * y) + (z * z)));
	}

	template<class T>
	Vector3<T> operator+(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return Vector3<T>(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y, aVectorA.z + aVectorB.z);
	}

	template<class T>
	Vector3<T> operator-(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return Vector3<T>(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y, aVectorA.z - aVectorB.z);
	}

	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template<class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}

	template<class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template<class T>
	Vector3<T> operator/(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return Vector3<T>(aVectorA.x / aVectorB.x, aVectorA.y / aVectorB.y, aVectorA.z / aVectorB.z);
	}

	template<class T>
	void operator+=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
		aVectorA.z += aVectorB.z;
	}

	template<class T>
	void operator-=(Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
		aVectorA.z -= aVectorB.z;
	}

	template<class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	template<class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		const T inverse = (1 / aScalar);
		aVector.x *= inverse;
		aVector.y *= inverse;
		aVector.z *= inverse;
	}

	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> vector;
		vector.x = aVector0.x * aVector1.x;
		vector.y = aVector0.y * aVector1.y;
		vector.z = aVector0.z * aVector1.z;

		return vector;
	}

	template <class T>
	std::ostream& operator<<(std::ostream& aOS, const Vector3<T>& aVector)
	{
		return aOS << "{X: " << aVector.x << " Y: " << aVector.y << " Z: " << aVector.z << "}";
	}
}