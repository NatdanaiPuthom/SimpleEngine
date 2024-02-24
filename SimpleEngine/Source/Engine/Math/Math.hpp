#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include <vector>

namespace Math
{
	constexpr float globalPi = 3.141592f;
	constexpr float globalDegToRad = globalPi / 180.0f;
	constexpr float globalRadToDeg = (180.0f / globalPi);

	template<typename T>
	static inline Vector4<T> AsVector4(const Vector3<T>& aVector3)
	{
		return Vector4<T>(aVector3.x, aVector3.y, aVector3.z, 1);
	}

	template<typename T>
	static inline T Clamp(const T aValue, const T aMin, const T aMax)
	{
		if (aValue < aMin)
			return aMin;

		if (aValue > aMax)
			return aMax;

		return aValue;
	}

	template<typename T>
	static inline void Swap(T& aFirstValue, T& aSecondValue)
	{
		const T swap = aFirstValue;
		aFirstValue = aSecondValue;
		aSecondValue = swap;
	}


	template<typename T>
	static inline T GetMax(const T& aFirstValue, const T& aSecondValue)
	{
		if (aFirstValue < aSecondValue)
			return aSecondValue;

		return aFirstValue;
	}


	template<typename T>
	static inline T GetMin(const T& aFirstValue, const T& aSecondValue)
	{
		if (aFirstValue < aSecondValue)
			return aFirstValue;

		return aSecondValue;
	}

	template<typename T>
	static inline T Distance(const Vector3<T>& aPositionA, const Vector3<T>& aPositionB)
	{
		return sqrt(pow(aPositionA.x - aPositionB.x, 2.0f) + pow(aPositionA.y - aPositionB.y, 2.0f) + pow(aPositionA.z - aPositionB.z, 2.0f));
	}

	template<typename T>
	static inline T Distance(const Vector2<T>& aPositionA, const Vector2<T>& aPositionB)
	{
		return sqrt(pow(aPositionA.x - aPositionB.x, 2.0f) + pow(aPositionA.y - aPositionB.y, 2.0f));
	}

	template<typename T>
	static inline T Dot(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return (aVectorA.x * aVectorB.x) + (aVectorA.y * aVectorB.y) + (aVectorA.z * aVectorB.z);
	}

	template<typename T>
	static inline T Dot(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		return (aVectorA.x * aVectorB.x) + (aVectorA.y * aVectorB.y);
	}

	template<typename T>
	static inline Vector3<T> Cross(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return
		{
			(aVectorA.y * aVectorB.z) - (aVectorA.z * aVectorB.y),
			(aVectorA.z * aVectorB.x) - (aVectorA.x * aVectorB.z),
			(aVectorA.x * aVectorB.y) - (aVectorA.y * aVectorB.x)
		};
	}

	template<typename T>
	static inline T Cross(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		const T crossProduct = aVectorA.x * aVectorB.y - aVectorA.y * aVectorB.x;
		return crossProduct;
	}

	template<typename T>
	static inline float CalculateAngle(Vector2<T> aPointA, Vector2<T> aPointB)
	{
		aPointA.Normalize();
		aPointB.Normalize();

		const float dotProduct = Math::Dot(aPointA, aPointB);
		const float crossProduct = Math::Cross(aPointB, aPointA);

		const float angleRadians = acos(dotProduct);
		float angleDegrees = angleRadians * globalRadToDeg;

		if (crossProduct < 0)
			angleDegrees = -angleDegrees;  // Flip the sign for clockwise rotations

		return angleDegrees;
	}

	template<typename T>
	static inline T Lerp(const T aStart, const T aTarget, float aPercent)
	{
		aPercent = Math::Clamp(aPercent, 0.0f, 1.0f);

		return aStart + aPercent * (aTarget - aStart);
	}

	template<typename T>
	static inline T SmoothStep(const T aStart, const T aTarget, float aPercent)
	{
		aPercent = Math::Clamp(aPercent, 0.0f, 1.0f);

		const float t = aPercent * aPercent * (3.0f - 2.0f * aPercent);
		return aStart + t * (aTarget - aStart);
	}

	template<typename T>
	static inline std::vector<std::vector<T>> SplitVector(const std::vector<T>& aOriginalVector, const size_t aSubVectorSize)
	{
		size_t numSubVectors = (aOriginalVector.size() + aSubVectorSize - 1) / aSubVectorSize;

		std::vector<std::vector<T>> subVectors(numSubVectors);

		for (size_t i = 0; i < numSubVectors; ++i)
		{
			size_t startIdx = i * aSubVectorSize;
			size_t endIdx = Math::GetMin((i + 1) * aSubVectorSize, aOriginalVector.size());

			subVectors[i].assign(aOriginalVector.begin() + startIdx, aOriginalVector.begin() + endIdx);
		}

		return subVectors;
	}
}