#pragma once
#include <memory>

namespace SimpleUtilities
{
	struct PairHash
	{
		template <typename T1, typename T2>
		std::size_t operator() (const std::pair<T1, T2>& aPair) const
		{
			auto hash1 = std::hash<T1>{}(aPair.first);
			auto hash2 = std::hash<T2>{}(aPair.second);

			return hash1 ^ hash2;
		}
	};

	struct PairEqual
	{
		template<typename T1, typename T2>
		bool operator() (const std::pair<T1, T2>& aPairOne, const std::pair<T1, T2>& aPairTwo) const
		{
			return aPairOne.first == aPairTwo.first && aPairOne.second == aPairTwo.second;
		}
	};
}