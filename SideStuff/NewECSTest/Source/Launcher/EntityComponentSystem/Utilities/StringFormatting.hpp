#pragma once
#include <string>
#include <algorithm>

namespace Simple
{
	template <size_t N>
	constexpr const char* ExtractVariableNameFromDataTypeName(const char(&name)[N])
	{
		for (size_t i = N - 1; i > 0; --i)
		{
			if (name[i - 1] == ':')
			{
				return name + i;
			}
		}

		return name;
	}

	static inline std::string ToLower(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
		return result;
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

	static inline std::string ConvertTypeIndexNameToPrettyName(const std::string& aNameFromTypeIndex)
	{
		size_t pos = aNameFromTypeIndex.find_last_of("::");

		if (pos != std::string::npos && pos + 1 < aNameFromTypeIndex.length())
		{
			return aNameFromTypeIndex.substr(pos + 1);
		}

		pos = aNameFromTypeIndex.find_last_of(" ");

		if (pos != std::string::npos)
		{
			return aNameFromTypeIndex.substr(pos + 1);
		}

		return aNameFromTypeIndex;
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
}