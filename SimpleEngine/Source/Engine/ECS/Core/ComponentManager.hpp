#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <cassert>
#include <any>

namespace ECS
{
	class EntityComponentSystem;
}

namespace ECS
{
	using ComponentID = size_t;
	using ComponentType = std::type_index;
	using EntityID = size_t;
	using ComponentHashCode = size_t;
	using ComponentVector = std::any;

	class ComponentManager final
	{
		friend class ECS::EntityComponentSystem;
	public:
		ComponentManager& operator=(const ComponentManager&) = default;
		ComponentManager(ComponentManager&&) = default;
		ComponentManager& operator=(ComponentManager&&) = default;

		template<typename T>
		void PrintMemoryPoolMemoryStateOfType() const;
	public:
		template<typename T>
		ComponentID CreateComponent(const EntityID aEntityID, const T& aComponent = T());
	public:
		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID, const ComponentID aComponentID);
	public:
		void* GetComponentByComponentID(const ComponentID aID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();

		template<typename T>
		T* GetComponentByComponentID(const ComponentID aID);

		template<typename T>
		const T* GetComponentByComponentID(const ComponentID aID) const;

		template<typename T>
		std::vector<T*> GetAllComponentsOfType();
	private:
		ComponentManager();
		~ComponentManager();
		ComponentManager(const ComponentManager& aOther);
	private:
		std::unordered_map<ComponentType, ComponentPool> myComponents;
		std::unordered_map<ComponentHashCode, ComponentVector> myCachedComponentVector;
		std::unordered_map<ComponentType, std::unordered_set<EntityID>> myComponentTypeToEntityIDs;
		std::unordered_map<ComponentID, ComponentType> myComponentIDToComponentTypeMap;
		size_t myCurrentComponentID;
		int myPadding[14] = { -1 };
	};

	template<typename T>
	inline void ComponentManager::PrintMemoryPoolMemoryStateOfType() const
	{
		myComponents.at(typeid(T)).PrintMemoryState();
	}

	template<typename T>
	inline ComponentID ComponentManager::CreateComponent(const EntityID aEntityID, const T& aComponent)
	{
		myCurrentComponentID++;

		if (!myComponents.contains(typeid(T)))
		{
			const std::string componentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
			myComponents.emplace(typeid(T), ComponentPool(16, componentName));
		}

		myComponents[typeid(T)].CreateComponent<T>(myCurrentComponentID, aComponent);

		myComponentTypeToEntityIDs[typeid(T)].insert(aEntityID);
		myComponentIDToComponentTypeMap.emplace(myCurrentComponentID, typeid(T));

		return myCurrentComponentID;
	}

	template<typename T>
	const std::unordered_set<EntityID>& ComponentManager::GetEntityIDsWithThisComponent()
	{
		return myComponentTypeToEntityIDs[typeid(T)];
	}

	template<typename T>
	inline T* ComponentManager::GetComponentByComponentID(const ComponentID aID)
	{
		if (myComponents.contains(typeid(T)) == false)
		{
			return nullptr;
		}

		const std::unordered_map<size_t, char*>& componentIDToPointerMap = myComponents.at(typeid(T)).GetComponentIDToPointerMap();
		auto it = componentIDToPointerMap.find(aID);

		if (it != componentIDToPointerMap.end())
		{
			return reinterpret_cast<T*>(it->second);
		}

		return nullptr;
	}

	template<typename T>
	inline const T* ComponentManager::GetComponentByComponentID(const ComponentID aID) const
	{
		if (myComponents.contains(typeid(T)) == false)
		{
			return nullptr;
		}

		const std::unordered_map<size_t, char*>& componentIDToPointerMap = myComponents.at(typeid(T)).GetComponentIDToPointerMap();
		auto it = componentIDToPointerMap.find(aID);

		if (it != componentIDToPointerMap.end())
		{
			return reinterpret_cast<T*>(it->second);
		}

		return nullptr;
	}

	template<typename T>
	inline std::vector<T*> ComponentManager::GetAllComponentsOfType()
	{
		if (myComponents.contains(typeid(T)) == false)
		{
			return std::vector<T*>();
		}

		auto it = myCachedComponentVector.find(typeid(T).hash_code());

		if (it != myCachedComponentVector.end())
		{
			return std::any_cast<std::vector<T*>>(it->second);
		}

		myCachedComponentVector.insert({ typeid(T).hash_code(), myComponents.at(typeid(T)).GetAllComponentsOfType<T>() });

		return std::any_cast<std::vector<T*>>(myCachedComponentVector.at(typeid(T).hash_code()));
	}
}