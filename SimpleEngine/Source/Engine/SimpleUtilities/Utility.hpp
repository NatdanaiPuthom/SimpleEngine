#pragma once
#include <string>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <vector>
#include <unordered_set>

namespace SimpleUtilities
{
	template<typename T>
	static inline std::vector<T> ReturnDifferenceBetweenVectors(const std::vector<T>& aVector1, const std::vector<T>& aVector2)
	{
		const std::unordered_set<T> setVec1(aVector1.begin(), aVector1.end());
		const std::unordered_set<T> setVec2(aVector2.begin(), aVector2.end());

		std::vector<T> missingElements;

		for (const auto& str : aVector1)
		{
			if (setVec2.find(str) == setVec2.end())
			{
				missingElements.push_back(str);
			}
		}

		for (const auto& str : aVector2)
		{
			if (setVec1.find(str) == setVec1.end())
			{
				missingElements.push_back(str);
			}
		}

		return missingElements;
	}

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

	static inline void AppendStringsInFront(const std::string& aStringToAdd, std::vector<std::string>& aOriginalString)
	{
		for (size_t i = 0; i < aOriginalString.size(); ++i)
		{
			aOriginalString[i] = aStringToAdd + aOriginalString[i];
		}
	}

	static inline void AppendStringInFront(const std::string& aStringToAdd, std::string& aOriginalString)
	{
		aOriginalString = aStringToAdd + aOriginalString;
	}

	static inline std::string AppendStringBeforeDot(const std::string& aStringToAdd, const std::string& aOriginalString)
	{
		const size_t dotPosition = aOriginalString.rfind('.');

		if (dotPosition != std::string::npos)
		{
			return std::string(aOriginalString.substr(0, dotPosition) + aStringToAdd + aOriginalString.substr(dotPosition, aOriginalString.length()));
		}

		return aOriginalString;
	}

	static inline std::vector<std::string> ReturnOnlyStringContaining(const std::string& aStringToSearch, const std::vector<std::string>& aOriginalString)
	{
		std::vector<std::string> filteredString;

		for (size_t i = 0; i < aOriginalString.size(); ++i)
		{
			const size_t pos = aOriginalString[i].find(aStringToSearch);

			if (pos != std::string::npos)
			{
				filteredString.push_back(aOriginalString[i]);
			}
		}

		return filteredString;
	}

	static inline std::string RemoveSubStringIfExist(const std::string& aOriginalString, const std::string& aStringToRemove)
	{
		std::string modifiedString = ToLower(aOriginalString);
		const size_t pos = modifiedString.find(ToLower(aStringToRemove));

		if (pos != std::string::npos)
		{
			modifiedString = aOriginalString;
			modifiedString.erase(pos, aStringToRemove.length());
			return modifiedString;
		}

		return aOriginalString;
	}

    static inline std::string ConvertAndAddSpaceToSubStringWithUpperCase(const std::string& aString)
    {
        if (aString.empty())
        {
            return aString;
        }

        std::string newString = aString;

		for (char& c : newString)
		{
			if (c == '_')
			{
				c = ' ';
			}
		}

        for (size_t i = 1; i < newString.length(); ++i)
        {
            if (std::isupper(static_cast<unsigned char>(newString[i])))
            {
                newString.insert(i, " ");
                ++i;
            }
        }

        newString[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(newString[0])));

        return newString;
    }
}