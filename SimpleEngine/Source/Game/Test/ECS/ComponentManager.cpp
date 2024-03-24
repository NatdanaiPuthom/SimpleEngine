#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test//ECS/ComponentManager.hpp"

namespace ECS
{
	ComponentManager::ComponentManager()
		: myCurrentComponentsAmount(0)
	{
	}

	ComponentManager::~ComponentManager()
	{
		for (auto& [key, value] : myComponents)
		{
			const std::size_t count = value.GetElementCount();

			if (count == 0)
				continue;

			const std::size_t size = value.GetSize() / count;

			char* components = value.GetStartMemoryAdress();

			for (size_t i = 0; i < count; ++i)
			{
				const std::size_t offset = i * size;

				myComponentDestructorInvoker[key](&components[offset]);
			}
		}
	}
}