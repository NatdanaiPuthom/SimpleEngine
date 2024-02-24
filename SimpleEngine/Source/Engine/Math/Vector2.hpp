#pragma once
#include <iostream>

namespace Math
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		Vector2<T>();
		~Vector2<T>() = default;

		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;

		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		bool operator==(const Vector2<T>& aVector) const;

		Vector2<T> GetNormalized() const;

		T LengthSqr() const;
		T Length() const;

		void Normalize();
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector2<unsigned int> Vector2ui;

	template <class T> Vector2<T> operator+(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB);
	template <class T> Vector2<T> operator-(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB);

	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector);

	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const float& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB);

	template <class T> void operator+=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB);
	template <class T> void operator-=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB);
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar);

	template <class T> std::ostream& operator<<(std::ostream& aOS, const Vector2<T>& aVector);

	template<class T>
	inline Vector2<T>::Vector2() : Vector2(0, 0)
	{}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY) : x(aX), y(aY)
	{}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		const T magnitude = (x * x) + (y * y);

		if (magnitude == 0 || magnitude == 1)
		{
			return *this;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));

		return Vector2<T>(x * normalize, y * normalize);
	}

	template<class T>
	inline void Vector2<T>::Normalize()
	{
		const T magnitude = static_cast<T> ((x * x) + (y * y));

		if (magnitude == 0 || magnitude == 1)
		{
			return;
		}

		const T normalize = T(1) / static_cast<T> (sqrt(magnitude));
		x *= normalize;
		y *= normalize;
	}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (x * x) + (y * y);
	}

	template<class T>
	inline T Vector2<T>::Length() const
	{
		return static_cast<T> (sqrt((x * x) + (y * y)));
	}

	template<class T>
	Vector2<T> operator+(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		return Vector2<T>(aVectorA.x + aVectorB.x, aVectorA.y + aVectorB.y);
	}

	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		return Vector2<T>(aVectorA.x - aVectorB.x, aVectorA.y - aVectorB.y);
	}

	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	template<class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar)
	{
		return { aVector.x / static_cast<T>(aScalar), aVector.y / static_cast<T>(aScalar) };
	}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const float& aScalar)
	{
		return { aVector.x / static_cast<T>(aScalar), aVector.y / static_cast<T>(aScalar) };
	}

	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		return Vector2<T>(aVectorA.x / aVectorB.x, aVectorA.y / aVectorB.y);
	}

	template<class T>
	void operator+=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		aVectorA.x += aVectorB.x;
		aVectorA.y += aVectorB.y;
	}

	template<class T>
	void operator-=(Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		aVectorA.x -= aVectorB.x;
		aVectorA.y -= aVectorB.y;
	}

	template<class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	template<class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		const T inverse = (1 / aScalar);
		aVector.x *= inverse;
		aVector.y *= inverse;
	}

	template<class T>
	inline bool Vector2<T>::operator==(const Vector2<T>& aVector) const
	{
		return (x == aVector.x && y == aVector.y);
	}

	template <class T>
	std::ostream& operator<<(std::ostream& aOS, const Vector2<T>& aVector)
	{
		return aOS << "{X: " << aVector.x << " Y: " << aVector.y << "}";
	}
}