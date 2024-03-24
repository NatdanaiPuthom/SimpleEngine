#pragma once
#include "Game/World.hpp"
#include <unordered_map>
#include <typeindex>

namespace ECS
{
	class Entity final
	{
	public:
		Entity();

		template<typename T>
		void AddComponent();

		template<typename T>
		void AddComponent(const T& aComponent);

		template<typename T>
		T* GetComponent();

		template<typename T>
		const T* GetComponent() const;
	private:
		std::unordered_map<std::type_index, size_t> myComponents;
	};

	template<typename T>
	inline void Entity::AddComponent()
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = World::GetComponentManager();
		myComponents[typeid(T)] = componentManager->GetComponentID(componentManager->CreateComponent<T>());
	}

	template<typename T>
	inline void Entity::AddComponent(const T& aComponent)
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = World::GetComponentManager();
		myComponents[typeid(T)] = componentManager->GetComponentID(componentManager->CreateComponent(aComponent));
	}

	template<typename T>
	inline T* Entity::GetComponent()
	{
		auto componentManager = World::GetComponentManager();
		return componentManager->GetComponentByID<T>(myComponents[typeid(T)]);
	}

	template<typename T>
	inline const T* Entity::GetComponent() const
	{
		auto componentManager = World::GetComponentManager();
		return componentManager->GetComponentByID<T>(myComponents[typeid(T)]);;
	}
}