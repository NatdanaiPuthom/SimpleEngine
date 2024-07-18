#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/ComponentManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace ECS
{
	ComponentManager::ComponentManager()
		: myCurrentComponentID(0)
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

			const size_t sizeOfComponentType = componentPool.GetComponentTypeSize();
			char* component = componentPool.GetStartMemoryAddress();

			for (size_t i = 0; i < componentCount; ++i)
			{	
				MainSingleton::GetComponentRegistry()->myTypeErasureComponentDestructorInvoker[componentType](&component[i * sizeOfComponentType]);;
			}
		}
	}

	bool ComponentManager::RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID, const ComponentID aComponentID)
	{
		ComponentPool& pool = myComponents[aComponentType];

		myComponentTypeToEntityIDs[aComponentType].erase(aEntityID);
		myComponentIDToComponentTypeMap.erase(aComponentID);

		return pool.SwapWithLastComponentAndRemove(aComponentID, aComponentType);
	}

	void* ComponentManager::GetComponentByComponentID(const ComponentID aID)
	{
		const auto& componentType = myComponentIDToComponentTypeMap.find(aID);

		if (componentType == myComponentIDToComponentTypeMap.end())
		{
			return nullptr;
		}

		auto& componentMap = myComponents[componentType->second].GetComponentIDToPointerMap();
		
		auto it = componentMap.find(aID);

		if (it != componentMap.end())
		{
			return reinterpret_cast<void*>(it->second);
		}
		
		return nullptr;
	}
}