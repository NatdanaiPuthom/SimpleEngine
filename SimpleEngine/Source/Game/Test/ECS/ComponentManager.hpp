#pragma once
#include "Game/Test/ECS/MemoryPool_ECS.hpp"
#include <typeindex>
#include <unordered_map>

namespace ECS
{
	class ComponentManager final
	{
	public:
		ComponentManager();
		~ComponentManager();

		template<typename T>
		T& AddComponent();

		template<typename T>
		T* GetComponentByID(const size_t aID);

		template<typename T>
		T* GetComponentByIndex(const size_t aIndex);

		template<typename T>
		size_t GetComponentID(const T& aComponent);

		template<typename T>
		size_t GetComponentCount();
	private:
		template<typename T>
		T& GetComponentByMemoryAdress(const char* aAdress);

		template<typename T>
		void RegisterDestructor();
	private:
		size_t myCurrentComponentsAmount;
		std::unordered_map<std::type_index, MemoryPool_ECS> myComponents;
		std::unordered_map<std::type_index, void (*)(void*)> myComponentDestructorInvoker;
		std::unordered_map<size_t, const char*> myAllComponents;
	};

	template<typename T>
	inline T& ComponentManager::AddComponent()
	{
		RegisterDestructor<T>(); //Only need to call once per Component Type but doesn't know how yet

		++myCurrentComponentsAmount;
		T& component = myComponents[typeid(T)].AllocateComponent<T>(myCurrentComponentsAmount);
		myAllComponents[myCurrentComponentsAmount] = reinterpret_cast<const char*>(&component);

		return component;
	}

	template<typename T>
	inline T* ComponentManager::GetComponentByID(const size_t aID)
	{
		auto it = myAllComponents.find(aID);

		if (it != myAllComponents.end())
		{
			return &GetComponentByMemoryAdress<T>(it->second);
		}

		return nullptr;
	}

	template<typename T>
	inline T* ComponentManager::GetComponentByIndex(const size_t aIndex)
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			return &it->second.GetValueByIndex<T>(aIndex);
		}

		return nullptr;
	}

	template<typename T>
	inline size_t ComponentManager::GetComponentID(const T& aComponent)
	{
		auto& it = myComponents[typeid(T)];

		const char* objectAdress = reinterpret_cast<const char*>(&aComponent);
		const size_t id = it.GetElementIDByMemoryAdress(objectAdress);

		return id;
	}

	template<typename T>
	inline size_t ComponentManager::GetComponentCount()
	{
		return myComponents[typeid(T)].GetElementCount();
	}

	template<typename T>
	inline T& ComponentManager::GetComponentByMemoryAdress(const char* aAdress)
	{
		return (T&)*(aAdress);
	}

	template<typename T>
	inline void ComponentManager::RegisterDestructor()
	{
		myComponentDestructorInvoker[typeid(T)] = [](void* pointer)
			{
				static_cast<T*>(pointer)->~T();
			};
	}
}