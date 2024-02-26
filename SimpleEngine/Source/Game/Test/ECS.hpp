#pragma once
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/world.hpp"
#include <typeindex>
#include <vector>
#include <cstring>
#include <unordered_map>

namespace Simple
{
	class ComponentManager
	{
		using ComponentID = std::type_index;
		using ComponentCount = std::size_t;
		using Components = std::vector<char>;
	public:
		ComponentManager() {};
		~ComponentManager() {};

		void SetWorldPointerToThis()
		{
			Impl::SimpleWorldComponentManager::SetComponentManager(this);
		};

		template<typename ComponentType>
		inline std::pair<std::size_t, ComponentType*> AddComponent(const ComponentType& aComponent)
		{
			auto it = myComponents.find(typeid(aComponent));

			if (it != myComponents.end())
			{
				ComponentCount& count = myComponents[typeid(aComponent)].first;
				Components& component = myComponents[typeid(aComponent)].second;

				if (component.size() <= count * sizeof(aComponent))
				{
					component.resize((count * 2) * sizeof(ComponentType));
				}

				std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
				++count;
			}
			else
			{
				ComponentCount& count = myComponents[typeid(aComponent)].first;
				Components& component = myComponents[typeid(aComponent)].second;

				if (component.size() <= count * sizeof(aComponent))
				{
					component.resize((count + 1) * sizeof(ComponentType));
				}

				std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
				++count;
			}

			const ComponentCount& count = myComponents[typeid(aComponent)].first - 1;
			return std::make_pair(static_cast<std::size_t>(count), GetComponent<ComponentType>(static_cast<std::size_t>(count)));
		}

		template<typename ComponentType>
		inline void RemoveComponent(const std::size_t aIndex)
		{
			ComponentCount& count = myComponents[typeid(ComponentType)].first;
			Components& components = myComponents[typeid(ComponentType)].second;

			ComponentType* componentPointer = reinterpret_cast<ComponentType*>(&components[aIndex * sizeof(ComponentType)]);
			componentPointer->~ComponentType();

			--count;
		}

		template<typename ComponentType>
		inline ComponentType* GetComponent(const std::size_t aIndex)
		{
			char* componentPointer = &myComponents[typeid(ComponentType)].second[aIndex * sizeof(ComponentType)];
			return reinterpret_cast<ComponentType*>(componentPointer);
		}

		template<typename ComponentType>
		inline std::vector<ComponentType>& GetAllComponentsOfType()
		{
			Components& componentBytes = myComponents[typeid(ComponentType)].second;

			const ComponentCount componentCount = myComponents[typeid(ComponentType)].first;
			const std::size_t elementCount = componentBytes.size() / sizeof(ComponentType);

			if (elementCount != componentCount)
			{
				componentBytes.resize(componentCount * sizeof(ComponentType));
			}

			return reinterpret_cast<std::vector<ComponentType>&>(componentBytes);
		}
	private:
		std::unordered_map<ComponentID, std::pair<ComponentCount, Components>> myComponents;
	};
}

class Entity
{
	using ComponentID = std::type_index;
	using ComponentIndex = std::size_t;
public:
	template<typename ComponentType>
	inline void AddComponent(const ComponentType& aComponent)
	{
		myComponents[typeid(ComponentType)] = World::GetComponentManager()->AddComponent<ComponentType>(aComponent);
	}

	template<typename ComponentType>
	inline ComponentType* GetComponent()
	{
		auto it = myComponents.find(typeid(ComponentType));

		if (it != myComponents.end())
		{
			return reinterpret_cast<ComponentType*>(it->second);
		}
		else
		{
			return nullptr;
		}
	}

	template<typename ComponentType>
	inline void RemoveComponentt()
	{
		auto it = myComponents.find(typeid(ComponentType));

		if (it != myComponents.end())
		{
			//World::GetComponentManager()->RemoveComponent<ComponentType>(it->first);
		}
	}
private:
	std::unordered_map<ComponentID, std::pair<ComponentIndex, void*>> myComponents;
};