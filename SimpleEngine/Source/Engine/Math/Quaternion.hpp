#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Math.hpp"

//Work In Progress

namespace Math
{
	template<typename T = float>
	class Quaternion final
	{
	public:
		T w;
		T x;
		T y;
		T z;

		Quaternion<T>();
		Quaternion<T>(const T& aW, const T& aX, const T& aY, const T& aZ);
		Quaternion<T>(const Vector3<T>& aVector);
		Quaternion<T>(const Vector3<T>& aVector, const T aAngle);
		Quaternion<T>(const Matrix4x4<T>& aMatrix);
	};

	typedef Quaternion<float> Quaternionf;

	template<typename T>
	inline Quaternion<T>::Quaternion()
	{
		w = static_cast<T>(1);
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const T& aW, const T& aX, const T& aY, const T& aZ)
		: w(aW)
		, x(aX)
		, y(aY)
		, z(aZ)
	{
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aVector)
	{
		const T cx = cos(aVector.X * T(0.5));
		const T cy = cos(aVector.Y * T(0.5));
		const T cz = cos(aVector.Z * T(0.5));
		const T sx = sin(aVector.X * T(0.5));
		const T sy = sin(aVector.Y * T(0.5));
		const T sz = sin(aVector.Z * T(0.5));

		w = cx * cy * cz + sx * sy * sz;

		x = sx * cy * cz - cx * sy * sz;
		y = cx * sy * cz + sx * cy * sz;
		z = cx * cy * sz - sx * sy * cz;
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aVector, const T aAngle)
	{
		const T halfAngle = aAngle / T(2);

		w = cos(halfAngle);

		const T halfAngleSin = sin(halfAngle);

		x = aVector.X * halfAngleSin;
		y = aVector.Y * halfAngleSin;
		z = aVector.Z * halfAngleSin;
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const Matrix4x4<T>& aMatrix)
	{
		w = std::sqrt(GetMax(T(0), T(1) + aMatrix(1,1) + aMatrix(2,2) + aMatrix(3,3))) * T(0.5);

		x = std::sqrt(GetMax(T(0), T(1) + aMatrix(1.1) - aMatrix(2,2) - aMatrix(3,3))) * T(0.5);
		y = std::sqrt(GetMax(T(0), T(1) - aMatrix(1.1) + aMatrix(2,2) - aMatrix(3,3))) * T(0.5);
		z = std::sqrt(GetMax(T(0), T(1) - aMatrix(1.1) - aMatrix(2,2) + aMatrix(3,3))) * T(0.5);

		x = std::copysign(x, aMatrix(3,2) - aMatrix(2,3));
		y = std::copysign(y, aMatrix(1,3) - aMatrix(3,1));
		z = std::copysign(z, aMatrix(2,1) - aMatrix(1,2));
	}
}