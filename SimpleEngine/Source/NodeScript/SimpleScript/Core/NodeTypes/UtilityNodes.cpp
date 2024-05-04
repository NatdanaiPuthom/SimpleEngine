#include "UtilityNodes.h"
#include "../Node/NodeTypeRegistry.h"

namespace SCR
{

	template<typename T, typename U>
	static U CastValueNode(T aValue)
	{
		return static_cast<U>(aValue);
	}

	template<typename T>
	static T ClampNode(T aValue, T aMin, T aMax)
	{
		return std::clamp(aValue, aMin, aMax);
	}

#undef min
#undef max

	template<typename T>
	static T MinNode(T aValue1, T aValue2)
	{
		return std::min(aValue1, aValue2);
	}

	template<typename T>
	static T MaxNode(T aValue1, T aValue2)
	{
		return std::max(aValue1, aValue2);
	}

	void RegisterUtilityNodes()
	{
		NodeTypeRegistry::RegisterNodeType(CastValueNode<int, float>, "Utility/Cast/Cast Int To Float");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<float, int>, "Utility/Cast/Cast Float To Int");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<int, unsigned int>, "Utility/Cast/Cast Int To Unsigned Int");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<short, int>, "Utility/Cast/Cast Short To Int");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<unsigned int, int>, "Utility/Cast/Cast Unsigned Int To Int");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<unsigned int, unsigned long long>, "Utility/Cast/Cast Unsigned Int To Unsigned Long Long");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<int, unsigned long long>, "Utility/Cast/Cast Int To Unsigned Long Long");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<unsigned long long, int>, "Utility/Cast/Cast Unsigned Long Long To Int");
		NodeTypeRegistry::RegisterNodeType(CastValueNode<unsigned long long, unsigned int>, "Utility/Cast/Cast Unsigned Long Long To Unsigned Int");

		NodeTypeRegistry::RegisterNodeType(ClampNode<int>, "Utility/Functions/Clamp (Int)");
		NodeTypeRegistry::RegisterNodeType(ClampNode<unsigned int>, "Utility/Functions/Clamp (Unsigned Int)");
		NodeTypeRegistry::RegisterNodeType(ClampNode<unsigned long long>, "Utility/Functions/Clamp (Unsigned Long Long)");
		NodeTypeRegistry::RegisterNodeType(ClampNode<float>, "Utility/Functions/Clamp (Float)");

		NodeTypeRegistry::RegisterNodeType(MinNode<int>, "Utility/Functions/Min (Int)");
		NodeTypeRegistry::RegisterNodeType(MinNode<float>, "Utility/Functions/Min (Float)");
		NodeTypeRegistry::RegisterNodeType(MinNode<unsigned long long>, "Utility/Functions/Min (Unsigned Long Long)");

		NodeTypeRegistry::RegisterNodeType(MaxNode<int>, "Utility/Functions/Max (Int)");
		NodeTypeRegistry::RegisterNodeType(MaxNode<float>, "Utility/Functions/Max (Float)");
		NodeTypeRegistry::RegisterNodeType(MaxNode<unsigned long long>, "Utility/Functions/Max (Unsigned Long Long)");

	}
}

