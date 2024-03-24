#pragma once
#include "Game/World.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>

namespace Simple
{
	class Entity final
	{
	public:
		Entity();

		template<typename T>
		bool AddComponent();

		template<typename T>
		bool AddComponent(const T& aComponent);

		template<typename T>
		bool RemoveComponent();

		template<typename T>
		T* GetComponent();

		template<typename T>
		const T* GetComponent() const;

		std::vector<size_t> GetAllComponentsID() const;
		std::vector<std::string> GetAllComponentsName() const;
	private:
		std::unordered_map<std::type_index, size_t> myComponents;
	};

	template<typename T>
	inline bool Entity::AddComponent()
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = World::GetComponentManager();

		const int componentID = componentManager->GetComponentID(componentManager->CreateComponent<T>());

		if (componentID < 0)
		{
			return false;
		}

		myComponents[typeid(T)] = componentID;
		return true;
	}

	template<typename T>
	inline bool Entity::AddComponent(const T& aComponent)
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = World::GetComponentManager();

		const int componentID = componentManager->GetComponentID(componentManager->CreateComponent(aComponent));

		if (componentID < 0)
		{
			return false;
		}

		myComponents[typeid(T)] = componentID;
		return true;
	}

	template<typename T>
	inline bool Entity::RemoveComponent()
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			World::GetComponentManager()->RemoveComponent<T>(it->second);
			return static_cast<bool>(myComponents.erase(typeid(T)));
		}

		return false;
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