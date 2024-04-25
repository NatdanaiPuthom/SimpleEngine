#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/ComponentM.hpp"

namespace Simple
{
	ComponentM::ComponentM()
	{
	}

	ComponentM::~ComponentM()
	{
		for (auto& [componentType, componentPool] : myComponents)
		{
			const size_t componentCount = componentPool.GetComponentCount();

			if (componentCount == 0)
			{
				continue;
			}

			const size_t sizeOfComponentType = componentPool.GetOccupiedMemorySpace() / componentCount;
			char* component = componentPool.GetStartMemoryAddress();

			for (size_t i = 0; i < componentCount; ++i)
			{
				myComponentDestructorInvoker[componentType](&component[i * sizeOfComponentType]);
			}
		}
	}
}