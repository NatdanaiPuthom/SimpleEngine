#pragma once
#include <iostream>
#include "Engine/Math/Vector3.hpp"

namespace Math
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		Vector4<T>();
		~Vector4<T>() = default;

		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& myW);
		Vector4<T>(const Vector4<T>& aVector) = default;

		Vector4<T>& operator=(const Vector4<T>& aVector2) = default;

		Vector3<T> AsVector3() const;
		Vector4<T> GetNormalized() const;

		T LengthSqr() const;
		T Length() const;
		T Dot(const Vector4<T>& aVector) const;

		void Normalize();
	};

	typedef Vector4<float> Vector4f;
	typedef Vector4<int> Vector4i;

	template <class T> Vector4<T> operator+(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB);
	template <class T> Vector4<T> operator-(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB);

	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar);
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector);

	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const int& aScalar);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const float& aScalar);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB);

	template <class T> void operator+=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB);
	template <class T> void operator-=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB);
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar);

	template <class T> std::ostream& operator<<(std::ostream& aOS, const Vector4<T>& aVector);

	template<class T>
	inline Vector4<T>::Vector4() : Vector4(0, 0, 0, 0)
	{}

	template<class T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) : x(aX), y(aY), z(aZ), w(aW)
	{}

	template<class T>
	inline Vector3<T> Vector4<T>::AsVector3() const
	{
		return Vector3<T>(x, y, z);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		const T magnitude = (x * x) + (y * y) + (z * z) + (w * w);

		if (magnitude == 0 || magnitude == 1)
		{
			return *this;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));

		return Vector4<T>(x * normalize, y * normalize, z * normalize, w * normalize);
	}

	template<class T>
	inline void Vector4<T>::Normalize()
	{
		const T magnitude = (x * x) + (y * y) + (z * z) + (w * w);

		if (magnitude == 0 || magnitude == 1)
		{
			return;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));
		x *= normalize;
		y *= normalize;
		z *= normalize;
		w *= normalize;
	}

	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	template<class T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T> (sqrt((x * x) + (y * y) + (z * z) + (w * w)));
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
	}

	template<class T>
	Vector4<T> operator+(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		return Vector4<T>(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y, aVectorA.z + aVectorB.z, +aVectorA.w + aVectorB.w);
	}

	template<class T>
	Vector4<T> operator-(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		return Vector4<T>(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y, aVectorA.z - aVectorB.z, aVectorA.w - aVectorB.w);
	}

	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	template<class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return  aVector * aScalar;
	}

	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const int& aScalar)
	{
		return { aVector.x / static_cast<T>(aScalar), aVector.y / static_cast<T>(aScalar), aVector.z / static_cast<T> (aScalar), aVector.w / static_cast<T> (aScalar) };
	}

	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const float& aScalar)
	{
		return { aVector.x / static_cast<T>(aScalar), aVector.y / static_cast<T>(aScalar), aVector.z / static_cast<T>(aScalar), aVector.w / static_cast<T> (aScalar) };
	}

	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		return Vector4<T>(aVectorA.x / aVectorB.x, aVectorA.y / aVectorB.y, aVectorA.z / aVectorB.z, aVectorA.w / aVectorB.w);
	}

	template<class T>
	void operator+=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
		aVectorA.z += aVectorB.z;
		aVectorA.w += aVectorB.w;
	}

	template<class T>
	void operator-=(Vector4<T>& aVectorA, const Vector4<T>& aVectorB)
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
		aVectorA.z -= aVectorB.z;
		aVectorA.w -= aVectorB.w;
	}

	template<class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	template<class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		const T inverse = (1 / aScalar);
		aVector.x *= inverse;
		aVector.y *= inverse;
		aVector.z *= inverse;
		aVector.w *= inverse;
	}

	template <class T>
	std::ostream& operator<<(std::ostream& aOS, const Vector4<T>& aVector)
	{
		return aOS << "{X: " << aVector.x << " Y: " << aVector.y << " Z: " << aVector.z << " W: " << aVector.w << "}";
	}
}