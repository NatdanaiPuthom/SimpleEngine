#pragma once
#include <string>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <filesystem>

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

	static inline std::string ConvertFilePathToPrettyName(const std::string& aFilePath, const bool aWithExtension = true)
	{
		size_t pos = aFilePath.find_last_of("/"); //NOTE(v9.34.0): I mostly use '/'

		if (pos != std::string::npos && pos + 1 < aFilePath.length())
		{
			std::string newFilePath = aFilePath.substr(pos + 1);

			if (aWithExtension == false)
			{
				pos = newFilePath.find_last_of('.');

				if (pos != std::string::npos)
				{
					newFilePath = newFilePath.substr(0, pos);
				}
			}

			return newFilePath;
		}

		pos = aFilePath.find_last_of("\\"); //NOTE(v9.35.0): Microsoft sure loves to use '\'

		if (pos != std::string::npos && pos + 1 < aFilePath.length())
		{
			std::string newFilePath = aFilePath.substr(pos + 1);

			if (aWithExtension == false)
			{
				pos = newFilePath.find_last_of('.');

				if (pos != std::string::npos)
				{
					newFilePath = newFilePath.substr(0, pos);
				}
			}

			return newFilePath;
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

	static inline const std::string ConvertAbsolutePathToRelativePath(const std::string& aFilePath)
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
			if (c == ':') //NOTE(v9.36.4): Assuming we found the symbol such as C: or D: as the ':' is special symbol
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
			const std::string absolutePath = SimpleUtilities::GetAbsolutePath(aFilePath);
			return absolutePath;
		}
	}

	static inline std::string ToLower(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
		return result;
	}

	//NOTE(v10.0.1): Case insensitive (doesn't matter upper or lower-case
	static inline bool FindSuffix(const std::string& aName, const std::string& aSuffixToSearch)
	{
		for (size_t i = aName.length(); i > 0; i--)
		{
			if (aName[i] == '.')
			{
				std::string suffix = aName.substr(i - 2, 2);

				if (ToLower(suffix) == ToLower(aSuffixToSearch))
				{
					return true;
				}

				break;
			}
		}

		return false;
	}

	static inline const std::string AppendCounterIfAlreadyExist(const std::string& aFileName)
	{
		std::string fileName = aFileName;
		const size_t dotPosition = aFileName.find_last_of('.');
		const std::string extension = aFileName.substr(dotPosition);

		int index = 0;

		do
		{
			fileName = aFileName.substr(0, dotPosition) + "(" + std::to_string(index++) + ")" + extension;

		} while (std::filesystem::exists(fileName));

		return fileName;
	}
}