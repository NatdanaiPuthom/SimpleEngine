#pragma once
#include "Game/Test/ECS2/ComponentM.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace Simple
{
	class EntityE;
}

namespace Simple
{
	class alignas(128) EntityM final
	{
	public:
		EntityM(ComponentM* aComponentManager);
		~EntityM();

		EntityE& CreateEntity();
		
		template<typename T>
		void AddComponent(const size_t aEntityID);

		template<typename T>
		T*& GetComponent(const size_t aEntityID);
		
	private:
		std::unordered_map<size_t, std::unordered_map<std::type_index, ComponentID>> myEntityComponents;
		std::vector<EntityE> myEntities;
		ComponentM* myComponentManager;
		char padding[6];
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