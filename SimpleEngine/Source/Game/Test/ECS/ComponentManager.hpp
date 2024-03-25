#pragma once
#include "Game/Test/ECS/MemoryPool_ECS.hpp"
#include <typeindex>
#include <unordered_map>

namespace Simple
{
	class ComponentManager final
	{
	public:
		ComponentManager();
		~ComponentManager();

		void Init();

		template<typename T>
		T& CreateComponent();

		template<typename T>
		T& CreateComponent(const T& aComponent);

		template<typename T>
		bool RemoveComponent(const size_t aComponentID);

		template<typename T>
		T* GetComponentByID(const size_t aID);

		template<typename T>
		T* GetComponentByIndex(const size_t aIndex);

		template<typename T>
		const T* GetComponentByID(const size_t aID) const;

		template<typename T>
		const T* GetComponentByIndex(const size_t aIndex) const;

		template<typename T>
		std::vector<T*> GetAllComponentsOfType();

		template<typename T>
		int GetComponentID(const T& aComponent);

		template<typename T>
		size_t GetComponentCount();
	private:
		template<typename T>
		T& GetComponentByMemoryAdress(const char* aAdress);

		template<typename T>
		void RegisterDestructor();

		void SetWorldPointerToThis();
	private:
		size_t myCurrentComponentsCount;
		std::unordered_map<std::type_index, MemoryPool_ECS> myComponents;
		std::unordered_map<std::type_index, void (*)(void*)> myComponentDestructorInvoker;
		std::unordered_map<size_t, const char*> myAllComponents; //TO-DO(v9.26.6): Remove or fix as when MemoryPool is re-allocating this will point to old adress
	};

	template<typename T>
	inline T& ComponentManager::CreateComponent()
	{
		if (myComponentDestructorInvoker.find(typeid(T)) == myComponentDestructorInvoker.end())
		{
			RegisterDestructor<T>();
		}

		++myCurrentComponentsCount;
		T& component = myComponents[typeid(T)].AllocateComponent<T>(myCurrentComponentsCount);
		myAllComponents[myCurrentComponentsCount] = reinterpret_cast<const char*>(&component);

		return component;
	}

	template<typename T>
	inline T& ComponentManager::CreateComponent(const T& aComponent)
	{
		if (myComponentDestructorInvoker.find(typeid(T)) == myComponentDestructorInvoker.end())
		{
			RegisterDestructor<T>();
		}

		++myCurrentComponentsCount;

		T& component = myComponents[typeid(T)].AllocateComponent<T>(myCurrentComponentsCount, aComponent);
		myAllComponents[myCurrentComponentsCount] = reinterpret_cast<const char*>(&component);

		return component;
	}

	template<typename T>
	inline bool ComponentManager::RemoveComponent(const size_t aComponentID)
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			T* component = GetComponentByID<T>(aComponentID);
			it->second.SwapWithLastAndRemove<T>(*component, aComponentID);

			myCurrentComponentsCount--;
			return myAllComponents.erase(aComponentID);
		}

		return false;
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
	inline const T* ComponentManager::GetComponentByID(const size_t aID) const
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
	inline const T* ComponentManager::GetComponentByIndex(const size_t aIndex) const
	{
		auto it = myComponents.find(typeid(T));

		if (it != myComponents.end())
		{
			return &it->second.GetValueByIndex<T>(aIndex);
		}

		return nullptr;
	}

	template<typename T>
	inline std::vector<T*> ComponentManager::GetAllComponentsOfType()
	{
		auto& it = myComponents[typeid(T)];
		const size_t count = it.GetElementCount();

		if (count == 0)
		{
			return {};
		}

		char* start = it.GetStartMemoryAddress();
		std::vector<T*> components(count);

		for (size_t i = 0; i < count; ++i)
		{
			components[i] = (T*)(start + i * sizeof(T));
		}

		return components;
	}

	template<typename T>
	inline int ComponentManager::GetComponentID(const T& aComponent)
	{
		auto& it = myComponents[typeid(T)];

		const char* objectAdress = reinterpret_cast<const char*>(&aComponent);
		const int id = it.GetElementIDByMemoryAddress(objectAdress);

		if (id < 0)
		{
			assert(false && "Component does not exist in memorypool");
		}

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