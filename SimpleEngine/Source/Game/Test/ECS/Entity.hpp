#pragma once
#include "Game/Test/ECS/Core/ECSNeedFix.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>

//NOTE(v9.26.6): AddComponent doesn't allow adding multiple components of same type for now, as it is easier debugging and testing. In future maybe only limiting to Transform and such special type

namespace Simple
{
	class Entity final
	{
	public:
		Entity();

		template<typename T>
		bool AddComponent(const T& aComponent = T());

		//TO-DO(v9.27): Fix some ref count for when removing the component
		template<typename T>
		bool AddSharedComponent(const size_t aComponentID);

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
	inline bool Entity::AddComponent(const T& aComponent)
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = Disgusting::GetComponentManager();

		myComponents[typeid(T)] = componentManager->CreateComponent<T>(aComponent);

		return true;
	}

	template<typename T>
	inline bool Entity::AddSharedComponent(const size_t aComponentID)
	{
		if (myComponents.find(typeid(T)) != myComponents.end())
		{
			assert(false && "Trying to add multiple components of same type is not allowed");
		}

		auto componentManager = Disgusting::GetComponentManager();

		const T* component = componentManager->GetComponentByID<T>(aComponentID);

		if (component == nullptr)
		{
			return false;
		}

		myComponents[typeid(T)] = aComponentID;

		return true;
	}

	template<typename T>
	inline bool Entity::RemoveComponent()
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			Disgusting::GetComponentManager()->RemoveComponent<T>(it->second);
			return static_cast<bool>(myComponents.erase(typeid(T)));
		}

		return false;
	}

	template<typename T>
	inline T* Entity::GetComponent()
	{
		auto componentManager = Disgusting::GetComponentManager();
		return componentManager->GetComponentByID<T>(myComponents[typeid(T)]);
	}

	template<typename T>
	inline const T* Entity::GetComponent() const
	{
		auto componentManager = Disgusting::GetComponentManager();
		return componentManager->GetComponentByID<T>(myComponents[typeid(T)]);;
	}
}