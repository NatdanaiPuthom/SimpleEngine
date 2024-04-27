#pragma once
#include "Game/Test/ECS2/CompPool.hpp"
#include <unordered_map>
#include <typeindex>

namespace Simple
{
	using ComponentID = size_t;
	using ComponentType = std::type_index;

	class ComponentM final
	{
	public:
		ComponentM();
		~ComponentM();

		template<typename T>
		ComponentID CreateComponent(const T& aComponent = T());

		template<typename T>
		T*& GetComponentByComponentID(const ComponentID aID);

	private:
		template<typename T>
		void RegisterDestructor();

	private:
		inline static size_t myCurrentComponentID = 0;

		std::unordered_map<ComponentType, void(*)(void*)> myComponentDestructorInvoker;
		std::unordered_map<ComponentType, ComponentPool> myComponents;
		std::unordered_map<size_t, char*> myAllComponents;
	};

	template<typename T>
	inline ComponentID ComponentM::CreateComponent(const T& aComponent)
	{
		if (myComponentDestructorInvoker.find(typeid(T)) == myComponentDestructorInvoker.end())
		{
			RegisterDestructor<T>();
		}

		myCurrentComponentID++;
		myAllComponents[myCurrentComponentID] = myComponents[typeid(T)].CreateComponent<T>(myCurrentComponentID, myAllComponents, aComponent);;

		return myCurrentComponentID;
	}

	template<typename T>
	inline T*& ComponentM::GetComponentByComponentID(const ComponentID aID)
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
	inline void ComponentM::RegisterDestructor()
	{
		myComponentDestructorInvoker[typeid(T)] = [](void* aPointer)
			{
				static_cast<T*>(aPointer)->~T();
			};
	}
}