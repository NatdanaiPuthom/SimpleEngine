#include "RandomNodes.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"
#include <random>

namespace FLY_NAMESPACE
{

	template<std::integral T>
	static T GetRandomNumber(const T aMin, const T aMax)
	{
		if (aMin > aMax)
		{
			return std::numeric_limits<T>::max();
		}
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<T> distr(aMin, aMax); // define the range

		return distr(gen);
	}

	REGISTER_FUNCTION(GetRandomNumber<int>, "Utility/Random");
	REGISTER_FUNCTION(GetRandomNumber<unsigned int>, "Utility/Random");
	REGISTER_FUNCTION(GetRandomNumber<unsigned long long>, "Utility/Random");

}
