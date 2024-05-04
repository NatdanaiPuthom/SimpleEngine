#include "RandomNodes.h"
#include "../Node/NodeTypeRegistry.h"
#include <random>

namespace SCR
{

	template<std::integral T>
	static T GetRandomNumberNode(const T aMin, const T aMax)
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

	void RegisterRandomNodes()
	{
		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<int>, "Utility/Random/Get Random Number (Int)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<unsigned int>, "Utility/Random/Get Random Number (Unsigned Int)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
		NodeTypeRegistry::RegisterNodeType(GetRandomNumberNode<unsigned long long>, "Utility/Random/Get Random Number (Unsigned Long Long)", NodeTypeDesc{ { "Min", "Max" }, { "Value" } });
	}

}
