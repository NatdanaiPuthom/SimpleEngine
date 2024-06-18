#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include <unordered_map>
#include <unordered_set>
#include <typeindex>

namespace ECS
{
	class EntityComponentSystem;
}

namespace ECS
{
	using ComponentID = size_t;
	using ComponentType = std::type_index;
	using EntityID = size_t;

	class ComponentManager final
	{
		friend class ECS::EntityComponentSystem;
	public:

		template<typename T>
		ComponentID CreateComponent(const EntityID aEntityID, const T& aComponent = T());

		template<typename T>
		bool RemoveComponent(const EntityID aEntityID, const ComponentID aComponentID);

		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID, const ComponentID aComponentID);

	public:
		void* GetComponentByComponentID(const ComponentID aID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();

		template<typename T>
		T*& GetComponentByComponentID(const ComponentID aID);

	private:
		ComponentManager();
		~ComponentManager();
	private:
		std::unordered_map<ComponentType, ComponentPool> myComponents;
		std::unordered_map<ComponentID, char*> myAllComponents;
		std::unordered_map<ComponentType, std::unordered_set<EntityID>> myComponentTypeToEntityIDs;
		size_t myCurrentComponentID;
		//const int myPaddings[2];
	};

	template<typename T>
	inline ComponentID ComponentManager::CreateComponent(const EntityID aEntityID, const T& aComponent)
	{
		myCurrentComponentID++;
		myAllComponents[myCurrentComponentID] = myComponents[typeid(T)].CreateComponent<T>(myCurrentComponentID, myAllComponents, aComponent);;
		myComponentTypeToEntityIDs[typeid(T)].insert(aEntityID);

		return myCurrentComponentID;
	}

	template<typename T>
	inline bool ComponentManager::RemoveComponent(const EntityID aEntityID, const ComponentID aComponentID)
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			T* component = GetComponentByComponentID<T>(aComponentID);
			bool success = it->second.SwapWithLastAndRemove<T>(*component, aComponentID);

			if (success == false)
			{
				assert(success && "Failed to Remove Component From Component Pool");
				return false;
			}

			myComponentTypeToEntityIDs[typeid(T)].erase(aEntityID);
			return myAllComponents.erase(aComponentID);
		}

		return false;
	}

	template<typename T>
	const std::unordered_set<EntityID>& ComponentManager::GetEntityIDsWithThisComponent()
	{
		return myComponentTypeToEntityIDs[typeid(T)];
	}

	template<typename T>
	inline T*& ComponentManager::GetComponentByComponentID(const ComponentID aID)
	{
		auto it = myAllComponents.find(aID);

		if (it != myAllComponents.end())
		{
			return reinterpret_cast<T*&>(it->second);
		}

		static T* nullPointer = nullptr;
		return std::ref(nullPointer);
	}
}