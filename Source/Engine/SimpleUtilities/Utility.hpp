#pragma once

constexpr float globalPi = 3.141592f; //Highest Precision For Floating Point
constexpr float globalDegToRad = globalPi / 180.0f;
constexpr float globalRadToDeg = (180.0f / globalPi);

namespace SimpleUtilities
{
	template<class T>
	class Vector2;

	template<class T>
	class Vector3;

	static inline std::wstring ToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	static inline const std::string GetPath(const char* aFilePath)
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		const std::string exePath(buffer);
		const std::string outputPath = exePath.substr(0, exePath.find_last_of("\\/")) + "//" + aFilePath;

		return outputPath;
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
	static inline T Lerp(const T aStart, const T aTarget, float aPercent)
	{
		aPercent = SimpleUtilities::Clamp(aPercent, 0.0f, 1.0f);

		return aStart + aPercent * (aTarget - aStart);
	}

	template<typename T>
	static inline T SmoothStep(const T aStart, const T aTarget, float aPercent)
	{
		aPercent = SimpleUtilities::Clamp(aPercent, 0.0f, 1.0f);

		const float t = aPercent * aPercent * (3.0f - 2.0f * aPercent);
		return aStart + t * (aTarget - aStart);
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
	static inline float CalculateAngle(Vector2<T> aPointA, Vector2<T> aPointB)
	{
		aPointA.Normalize();
		aPointB.Normalize();

		const float dotProduct = aPointA.Dot(aPointB);
		const float crossProduct = aPointB.Cross(aPointA);

		const float angleRadians = acos(dotProduct);
		float angleDegrees = angleRadians * globalRadToDeg;

		if (crossProduct < 0)
			angleDegrees = -angleDegrees;  // Flip the sign for clockwise rotations

		return angleDegrees;
	}

	template<typename T>
	static inline T Dot(const Vector2<T>& aVectorA, const Vector2<T>& aVectorB)
	{
		return (aVectorA.x * aVectorB.x) + (aVectorA.y * aVectorB.y);
	}

	template<typename T>
	static inline T Dot(const Vector3<T>& aVectorA, const Vector3<T>& aVectorB)
	{
		return (aVectorA.x * aVectorB.x) + (aVectorA.y * aVectorB.y) + (aVectorA.z * aVectorB.z);
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
}
