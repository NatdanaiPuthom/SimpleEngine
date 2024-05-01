#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/ComponentManager.hpp"

namespace Simple
{
	ComponentManager::ComponentManager()
	{
	}

	ComponentManager::~ComponentManager()
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

	bool ComponentManager::RemoveComponentByTypeIndex(const ComponentType& aComponentType, const size_t aComponentID)
	{
		ComponentPool& pool = myComponents[aComponentType]; pool;
		aComponentType; aComponentID;
		return false;
	}

	const std::type_index ComponentManager::GetComponentTypeIndexByName(const std::string& aComponentTypeName)
	{
		const auto it = myComponentNameToTypeIndex.find(aComponentTypeName);

		if (it != myComponentNameToTypeIndex.end())
		{
			return it->second;
		}

		return std::type_index(typeid(NullComponent));
	}

	std::type_index ComponentManager::GetTypeIndexByName(const ComponentName aComponentName)
	{
		auto it = myComponentNameToTypeIndex.find(aComponentName);

		if (it != myComponentNameToTypeIndex.end())
		{
			return it->second;
		}

		return std::type_index(typeid(NullComponent));
	}
}