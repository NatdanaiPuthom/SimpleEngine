#pragma once

namespace Fly
{
	class DataTypeFacade;
}

namespace ImGui
{
	
	template<size_t MaxSize>
	bool InputString(const char* aText, std::string& aString)
	{
		char buffer[MaxSize]{};
		strcpy_s(buffer, aString.c_str());
		if (ImGui::InputText(aText, buffer, MaxSize))
		{
			aString = buffer;
			return true;
		}

		return false;
	}
}

namespace Editor
{
	template<typename T>
	constexpr std::string Combine(const char* aName, const T& aValue)
	{
		return std::string(aName) + std::to_string(aValue);
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);
	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);
	bool DataTypeComboNoFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);

	static bool StringCompare(std::string_view aStr1, std::string_view aStr2)
	{
		auto it = std::search(
			aStr1.begin(), aStr1.end(),
			aStr2.begin(), aStr2.end(),
			[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return it != aStr1.end();
	}

	inline constexpr unsigned int ToImGuiColor(const Fly::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}
	
	template<typename T>
	concept Iteratable = requires(T && a)
	{
		{ begin(a) };
		{ end(a) };
	};

	template<Iteratable T, Iteratable U>
	inline bool SearchString(T&& aSearchIn, U&& aSearchFor)
	{
		auto it = std::search(begin(aSearchIn), end(aSearchIn), std::boyer_moore_horspool_searcher(begin(aSearchFor), end(aSearchFor)));

		return it != end(aSearchIn);
	}
}
