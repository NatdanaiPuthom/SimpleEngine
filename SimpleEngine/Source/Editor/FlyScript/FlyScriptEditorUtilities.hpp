#pragma once

namespace Fly
{
	class DataTypeFacade;
}

namespace Editor
{

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);
	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::DataTypeFacade& aDataTypeFacade);


	static bool StringCompare(std::string_view aStr1, std::string_view aStr2)
	{
		auto it = std::search(
			aStr1.begin(), aStr1.end(),
			aStr2.begin(), aStr2.end(),
			[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return it != aStr1.end();
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
