#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test//ECS/Core/ComponentManager.hpp"
#include "ComponentManager.hpp"
#include "ComponentManager.hpp"

namespace Simple
{
	ComponentManager::ComponentManager()
		: myCurrentComponentsCount(0)
	{
	}

	ComponentManager::~ComponentManager()
	{
		for (auto& [key, value] : myComponents)
		{
			const std::size_t count = value.GetComponentCount();

			if (count == 0)
				continue;

			const std::size_t size = value.GetSize() / count;

			char* components = value.GetStartMemoryAddress();

			for (size_t i = 0; i < count; ++i)
			{
				const std::size_t offset = i * size;

				myComponentDestructorInvoker[key](&components[offset]);
			}
		}
	}
}