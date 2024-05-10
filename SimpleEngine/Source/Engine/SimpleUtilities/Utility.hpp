#pragma once
#include <string>
#include <Windows.h>

namespace SimpleUtilities
{
	static inline std::wstring ToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	static inline std::string ConvertTypeIndexNameToPrettyName(const std::string& aNameFromTypeIndex)
	{
		size_t pos = aNameFromTypeIndex.find_last_of("::"); //NOTE(v9.30.10): For struct/classes with namespaces
		
		if (pos != std::string::npos && pos + 1 < aNameFromTypeIndex.length())
		{
			return aNameFromTypeIndex.substr(pos + 1);
		}

		pos = aNameFromTypeIndex.find_last_of(" "); //NOTE(v9.30.10): For struct/classes without namespaces

		if (pos != std::string::npos)
		{
			return aNameFromTypeIndex.substr(pos + 1);
		}
	
		return aNameFromTypeIndex;
	}

	static inline std::string ConvertFilePathToPrettyName(const std::string& aFilePath)
	{
		size_t pos = aFilePath.find_last_of("/"); //NOTE(v9.34.0): I mostly use '/'

		if (pos != std::string::npos && pos + 1 < aFilePath.length())
		{
			return aFilePath.substr(pos + 1);
		}

		pos = aFilePath.find_last_of("\\"); //NOTE(v9.35.0): Microsoft sure loves to use '\'

		if (pos != std::string::npos && pos + 1 < aFilePath.length())
		{
			return aFilePath.substr(pos + 1);
		}

		return aFilePath;
	}

	static inline const std::string GetAbsolutePath(const std::string& aFilePath)
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		const std::string exePath(buffer);
		const std::string outputPath = exePath.substr(0, exePath.find_last_of("\\")) + "\\" + aFilePath;

		return outputPath;
	}

	static inline const std::string KeepStringAfterAssets(const std::string& aFilePath)
	{
		const size_t pos = aFilePath.rfind("Assets");

		if (pos != std::string::npos)
		{
			return aFilePath.substr(pos);
		}
		else
		{
			return "";
		}
	}

	static inline const std::string CheckAndReturnAsAbsolutePath(const std::string& aFilePath)
	{
		bool isAbsolutePath = false;

		for (const char c : aFilePath)
		{
			if (c == ':')
			{
				isAbsolutePath = true;
				break;
			}
		}

		if (isAbsolutePath == true)
		{
			return aFilePath;
		}
		else
		{
			const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_MODELS) + aFilePath;
			return absolutePath;
		}
	}
}