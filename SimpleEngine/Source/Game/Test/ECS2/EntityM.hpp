#pragma once
#include "Game/Test/ECS2/ComponentM.hpp"
#include "Game/Test/ECS2/EntityP.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace Simple
{
	class EntityE;
}

namespace Simple
{
	class EntityM final
	{
		using EntityID = size_t;
		using ComponentType = std::type_index;
	public:
		EntityM(ComponentM* aComponentManager);
		~EntityM();

		EntityE*& CreateEntity();
		
		template<typename T>
		void AddComponent(const size_t aEntityID);

		template<typename T>
		T*& GetComponent(const size_t aEntityID);
		
	private:
		static size_t myCurrentEntityID;
		EntityP myEntityPool;
		ComponentM* myComponentManager;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;
		std::unordered_map<EntityID, char*> myEntities;
	};

	template<typename T>
	inline void EntityM::AddComponent(const size_t aEntityID)
	{
		myEntityComponents[aEntityID][typeid(T)] = myComponentManager->CreateComponent<T>();
	}

	template<typename T>
	inline T*& EntityM::GetComponent(const size_t aEntityID)
	{
		return myComponentManager->GetComponentByComponentID<T>(myEntityComponents[aEntityID][typeid(T)]);
	}
}