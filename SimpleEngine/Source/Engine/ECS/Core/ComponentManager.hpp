#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/Components/Core/NullComponent.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include <unordered_map>
#include <typeindex>
#include <string>

namespace Simple
{
	class ECS;
}

namespace Simple
{
	using ComponentID = size_t;
	using ComponentType = std::type_index;

	class ComponentManager final
	{
		using ComponentName = std::string;
		friend class Simple::ECS;
	public:
		~ComponentManager();

		template<typename T>
		ComponentID CreateComponent(const T& aComponent = T());

		template<typename T>
		bool RemoveComponent(const size_t aComponentID);

		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const size_t aComponentID);

		const std::type_index GetComponentTypeIndexByName(const std::string& aComponentTypeName);

		template<typename T>
		T*& GetComponentByComponentID(const ComponentID aID);

		std::type_index GetTypeIndexByName(const ComponentName aComponentName);

	private:
		ComponentManager();

		template<typename T>
		void RegisterDestructor();
	private:
		inline static size_t myCurrentComponentID = 0;

		std::unordered_map<ComponentType, void(*)(void*)> myComponentDestructorInvoker;
		std::unordered_map<ComponentName, ComponentType> myComponentNameToTypeIndex;
		std::unordered_map<ComponentType, ComponentPool> myComponents;
		std::unordered_map<ComponentID, char*> myAllComponents;
	};

	template<typename T>
	inline ComponentID ComponentManager::CreateComponent(const T& aComponent)
	{
		if (myComponentDestructorInvoker.find(typeid(T)) == myComponentDestructorInvoker.end())
		{
			RegisterDestructor<T>();

			const std::type_index typeIndex = typeid(T);
			std::string componentName = typeIndex.name();
			componentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(componentName);
			myComponentNameToTypeIndex.emplace(componentName, typeIndex);
		}

		myCurrentComponentID++;
		myAllComponents[myCurrentComponentID] = myComponents[typeid(T)].CreateComponent<T>(myCurrentComponentID, myAllComponents, aComponent);;

		return myCurrentComponentID;
	}

	template<typename T>
	inline bool ComponentManager::RemoveComponent(const size_t aComponentID)
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			T* component = GetComponentByComponentID<T>(aComponentID);
			const bool success = it->second.SwapWithLastAndRemove<T>(*component, aComponentID);
			assert(success && "Failed to Remove Component From Component Pool");

			return myAllComponents.erase(aComponentID);
		}

		return false;
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

	template<typename T>
	inline void ComponentManager::RegisterDestructor()
	{
		myComponentDestructorInvoker[typeid(T)] = [](void* aPointer)
			{
				static_cast<T*>(aPointer)->~T();
			};
	}
}