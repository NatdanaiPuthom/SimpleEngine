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
		myComponents.clear();
	}

	ComponentManager::ComponentManager(const ComponentManager& aOther)
		: myComponents(aOther.myComponents)
		, myComponentTypeToEntityIDs (aOther.myComponentTypeToEntityIDs)
		, myComponentIDToComponentTypeMap(aOther.myComponentIDToComponentTypeMap)
		, myCurrentComponentID(aOther.myCurrentComponentID)
	{
		strcpy_s(myPadding, aOther.myPadding);
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