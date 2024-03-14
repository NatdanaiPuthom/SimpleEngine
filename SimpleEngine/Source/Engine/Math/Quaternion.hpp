#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Math.hpp"
#include <iostream>

//Work In Progress

namespace Math
{
	template <typename T>
	class Matrix4x4;

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

		Quaternion<T>& operator=(const Quaternion<T>& aQuat) = default;

		inline void Normalize();
		inline Quaternion<T> GetNormalized() const;

		T Length() const;
		T LengthSquared() const;
		inline T Dot(const Quaternion<T>& aQuaternion) const;

		inline Matrix4x4<T> GetRotationMatrix4x4() const;
		inline Vector3<T> GetForward() const;
		inline Vector3<T> GetRight() const;
		inline Vector3<T> GetUp() const;

		inline static Vector3<T> RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3<T>& aVectorToRotate);
		inline static Quaternion<T> Lerp(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB, const T& aDelta);
		inline static Quaternion<T> Slerp(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB, const T& aDelta);
	};

	typedef Quaternion<float> Quaternionf;

	template <class T> std::ostream& operator<<(std::ostream& aOS, const Quaternion<T>& aQuaternion);
	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuaternion, const T& aScalar);
	template <class T> inline Quaternion<T> operator*(const T& aScalar, const Quaternion<T>& aQuaternion);
	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB);
	template <class T> inline void operator*=(Quaternion<T>& aQuaternion, const T& aScalar);
	template <class T> inline void operator*=(Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB);
	template <class T> inline Quaternion<T> operator/(const Quaternion<T>& aQuaternion, const T& aScalar);
	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB);
	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuaternion);
	template <class T> inline Quaternion<T> operator+(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB);
	template <class T> inline Quaternion<T> operator+=(Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB);

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
		const T cx = std::cos(aVector.x * T(0.5));
		const T cy = std::cos(aVector.y * T(0.5));
		const T cz = std::cos(aVector.z * T(0.5));
		const T sx = std::sin(aVector.x * T(0.5));
		const T sy = std::sin(aVector.y * T(0.5));
		const T sz = std::sin(aVector.z * T(0.5));

		w = cx * cy * cz + sx * sy * sz;

		x = sx * cy * cz - cx * sy * sz;
		y = cx * sy * cz + sx * cy * sz;
		z = cx * cy * sz - sx * sy * cz;
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aVector, const T aAngle)
	{
		const T halfAngle = aAngle / T(2);

		w = std::cos(halfAngle);

		const T halfAngleSin = sin(halfAngle);

		x = aVector.x * halfAngleSin;
		y = aVector.y * halfAngleSin;
		z = aVector.z * halfAngleSin;
	}

	template<typename T>
	inline Quaternion<T>::Quaternion(const Matrix4x4<T>& aMatrix)
	{
		w = std::sqrt(GetMax(T(0), T(1) + aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3))) * T(0.5);

		x = std::sqrt(GetMax(T(0), T(1) + aMatrix(1, 1) - aMatrix(2, 2) - aMatrix(3, 3))) * T(0.5);
		y = std::sqrt(GetMax(T(0), T(1) - aMatrix(1, 1) + aMatrix(2, 2) - aMatrix(3, 3))) * T(0.5);
		z = std::sqrt(GetMax(T(0), T(1) - aMatrix(1, 1) - aMatrix(2, 2) + aMatrix(3, 3))) * T(0.5);

		x = std::copysign(x, aMatrix(3, 2) - aMatrix(2, 3));
		y = std::copysign(y, aMatrix(1, 3) - aMatrix(3, 1));
		z = std::copysign(z, aMatrix(2, 1) - aMatrix(1, 2));
	}

	template<class T>
	std::ostream& operator<<(std::ostream& aOS, const Quaternion<T>& aQuaternion)
	{
		return aOS << "{ W: " << aQuaternion.w << " X: " << aQuaternion.x << " Y: " << aQuaternion.y << " Z: " << aQuaternion.z << " }" << std::endl;
	}

	template<class T>
	Quaternion<T> operator*(const Quaternion<T>& aQuaternion, const T& aScalar)
	{
		return Quaternion<T>(aQuaternion.w * aScalar, aQuaternion.x * aScalar, aQuaternion.y * aScalar, aQuaternion.z * aScalar);
	}

	template<class T>
	Quaternion<T> operator*(const T& aScalar, const Quaternion<T>& aQuaternion)
	{
		return Quaternion<T>(aQuaternion.w * aScalar, aQuaternion.x * aScalar, aQuaternion.y * aScalar, aQuaternion.z * aScalar);
	}

	template<class T>
	Quaternion<T> operator*(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB)
	{
		return Quaternion<T>(
			(aQuaternionB.w * aQuaternionA.w) - (aQuaternionB.x * aQuaternionA.x) - (aQuaternionB.y * aQuaternionA.y) - (aQuaternionB.z * aQuaternionA.z),
			(aQuaternionB.w * aQuaternionA.x) + (aQuaternionB.x * aQuaternionA.w) + (aQuaternionB.y * aQuaternionA.z) - (aQuaternionB.z * aQuaternionA.y),
			(aQuaternionB.w * aQuaternionA.y) + (aQuaternionB.y * aQuaternionA.w) + (aQuaternionB.z * aQuaternionA.x) - (aQuaternionB.x * aQuaternionA.z),
			(aQuaternionB.w * aQuaternionA.z) + (aQuaternionB.z * aQuaternionA.w) + (aQuaternionB.x * aQuaternionA.y) - (aQuaternionB.y * aQuaternionA.x)
		);
	}

	template<class T>
	void operator*=(Quaternion<T>& aQuaternion, const T& aScalar)
	{
		aQuaternion.w *= aScalar;
		aQuaternion.x *= aScalar;
		aQuaternion.y *= aScalar;
		aQuaternion.z *= aScalar;
	}

	template<class T>
	void operator*=(Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB)
	{
		const T w = aQuaternionA.w;
		const T x = aQuaternionA.x;
		const T y = aQuaternionA.y;
		const T z = aQuaternionA.z;

		aQuaternionA.w = (aQuaternionB.w * w) - (aQuaternionB.x * x) - (aQuaternionB.y * y) - (aQuaternionB.z * z);
		aQuaternionA.x = (aQuaternionB.w * x) + (aQuaternionB.x * w) + (aQuaternionB.y * z) - (aQuaternionB.z * y);
		aQuaternionA.y = (aQuaternionB.w * y) + (aQuaternionB.y * w) + (aQuaternionB.z * x) - (aQuaternionB.x * z);
		aQuaternionA.z = (aQuaternionB.w * z) + (aQuaternionB.z * w) + (aQuaternionB.x * y) - (aQuaternionB.y * x);
	}

	template<class T>
	Quaternion<T> operator/(const Quaternion<T>& aQuaternion, const T& aScalar)
	{
		return Quaternion<T>(aQuaternion.w / aScalar, aQuaternion.x / aScalar, aQuaternion.y / aScalar, aQuaternion.z / aScalar);
	}

	template<class T>
	Quaternion<T> operator-(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB)
	{
		return Quaternion<T>(aQuaternionA.w - aQuaternionB.w, aQuaternionA.x - aQuaternionB.x, aQuaternionA.y - aQuaternionB.y, aQuaternionA.z - aQuaternionB.z);
	}

	template<class T>
	Quaternion<T> operator-(const Quaternion<T>& aQuaternion)
	{
		return Quaternion<T>(-aQuaternion.w, -aQuaternion.x, -aQuaternion.y, -aQuaternion.z);
	}

	template<class T>
	Quaternion<T> operator+(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB)
	{
		return Quaternion<T>(aQuaternionA.w + aQuaternionB.w, aQuaternionA.x + aQuaternionB.x, aQuaternionA.y + aQuaternionB.y, aQuaternionA.z + aQuaternionB.z);
	}

	template<class T>
	Quaternion<T> operator+=(Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB)
	{
		aQuaternionA.w += aQuaternionB.w;
		aQuaternionA.x += aQuaternionB.x;
		aQuaternionA.y += aQuaternionB.y;
		aQuaternionA.z += aQuaternionB.z;
	}

	template<typename T>
	inline void Quaternion<T>::Normalize()
	{
		const T length = T(1) / Length();

		w *= length;

		x *= length;
		y *= length;
		z *= length;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::GetNormalized() const
	{
		const T length = T(1) / Length();

		return Quaternion<T>(w * length, x * length, y * length, z * length);
	}

	template<typename T>
	inline T Quaternion<T>::Length() const
	{
		return std::sqrt(LengthSquared());
	}

	template<typename T>
	inline T Quaternion<T>::LengthSquared() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	template<typename T>
	inline T Quaternion<T>::Dot(const Quaternion<T>& aQuaternion) const
	{
		return x * aQuaternion.x + y * aQuaternion.y + z * aQuaternion.z + w * aQuaternion.w;
	}

	template<typename T>
	inline Matrix4x4<T> Quaternion<T>::GetRotationMatrix4x4() const
	{
		Matrix4x4<T> result;

		const T qxx(x * x);
		const T qyy(y * y);
		const T qzz(z * z);

		const T qxz(x * z);
		const T qxy(x * y);
		const T qyz(y * z);

		const T qwx(w * x);
		const T qwy(w * y);
		const T qwz(w * z);

		//TO-DO: Check if this is row or col major, I may have fucked up
		result(1, 1) = T(1) - T(2) * (qyy + qzz);
		result(2, 1) = T(2) * (qxy + qwz);
		result(3, 1) = T(2) * (qxz - qwy);

		result(1, 2) = T(2) * (qxy - qwz);
		result(2, 2) = T(1) - T(2) * (qxx + qzz);
		result(3, 2) = T(2) * (qyz + qwx);

		result(1, 3) = T(2) * (qxz + qwy);
		result(2, 3) = T(2) * (qyz - qwx);
		result(3, 3) = T(1) - T(2) * (qxx * qyy);

		return result;
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetForward() const
	{
		const Vector3<T> forward(0.0f, 0.0f, 1.0f);
		return Quaternion<T>::RotateVectorByQuaternion(*this, forward);
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetRight() const
	{
		const Vector3<T> right(1.0f, 0.0f, 0.0f);
		return Quaternion<T>::RotateVectorByQuaternion(*this, right);
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetUp() const
	{
		const Vector3<T> up(0.0f, 1.0f, 0.0f);
		return Quaternion<T>::RotateVectorByQuaternion(*this, up);
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3<T>& aVectorToRotate)
	{
		const Vector3<T> vector(aQuaternion.x, aQuaternion.y, aQuaternion.z);

		const Vector3<T> result = 2.0f * vector.Dot(aVectorToRotate) * vector
			+ (aQuaternion.w * aQuaternion.w - vector.Dot(vector)) * aVectorToRotate
			+ 2.0f * aQuaternion.w * vector.Cross(aVectorToRotate);

		return result;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::Lerp(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB, const T& aDelta)
	{
		Quaternion<T> result;

		const float deltaInverse = 1 - aDelta;

		result.w = deltaInverse * aQuaternionA.w + aDelta * aQuaternionB.w;

		result.x = deltaInverse * aQuaternionA.x + aDelta * aQuaternionB.x;
		result.y = deltaInverse * aQuaternionA.y + aDelta * aQuaternionB.y;
		result.z = deltaInverse * aQuaternionA.z + aDelta * aQuaternionB.z;

		result.Normalize();

		return result;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& aQuaternionA, const Quaternion<T>& aQuaternionB, const T& aDelta)
	{
		Quaternion<T> qz = aQuaternionB;

		T cosTheta = aQuaternionA.Dot(aQuaternionB);

		if (cosTheta < T(0))
		{
			qz = -qz;
			cosTheta = -cosTheta;
		}

		const T dotThreshold = static_cast<T>(0.9995);

		if (cosTheta > T(1) - dotThreshold)
		{
			return Lerp(aQuaternionA, qz, aDelta);
		}

		const T angle = acos(cosTheta);
		return (std::sin((T(1) - aDelta) * angle) * aQuaternionA + std::sin(aDelta * angle) * qz) / std::sin(angle);
	}
}