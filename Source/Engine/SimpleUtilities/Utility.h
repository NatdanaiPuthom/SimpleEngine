#pragma once
#include <External/profiler.h>

constexpr float globalPi = 3.141592f; //Highest Precision For Floating Point

namespace SimpleUtilities
{
	template<typename T>
	class Vector3;

	template<typename T>
	class Vector4;

	template<typename T>
	inline Vector3<T> DegreeToRad(const Vector3<T>& aRotation)
	{
		const Vector3<T> rotationInRad = aRotation * (globalPi / 180.0f);
		return rotationInRad;
	}

	template<typename T>
	inline Vector3<T> RadToDegree(const Vector3<T>& aRotation)
	{
		const Vector3<T> rotationInDegree = aRotation * (180.0f / globalPi);
		return rotationInDegree;
	}

	template<typename T>
	static inline Vector3<T> ToVector3(const Vector4<T>& aVector)
	{
		return Vector3<T>(aVector.x, aVector.y, aVector.z);
	}

	template<typename T>
	static inline T Lerp(const T aStart, const T aEnd, const float aPercent)
	{
		return aStart + aPercent * (aEnd - aStart);
	}

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
}
