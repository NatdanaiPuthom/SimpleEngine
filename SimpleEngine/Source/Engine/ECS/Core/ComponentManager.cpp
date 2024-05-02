#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/ComponentManager.hpp"

namespace ECS
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
		char* component = pool.GetComponentAddressByID(aComponentID);
		char* start = pool.GetStartMemoryAddress(); start;
		const size_t size = pool.GetComponentTypeSize();
		const int index = pool.GetComponentIndexByMemoryAddress(component, pool.GetComponentTypeSize());

		if (index < 0)
		{
			assert(false && "Failed to Remove Component from the Pool");
			return false;
		}

		myComponentDestructorInvoker[aComponentType](&component[index * size]);
		myAllComponents.erase(aComponentID);

		return pool.SwapWithLastAndRemoveEditor(aComponentID);
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