#pragma once
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/world.hpp"
#include <typeindex>
#include <vector>
#include <cstring>
#include <string>
#include <unordered_map>

namespace Simple
{
	class ComponentManager final
	{
		using ComponentID = std::type_index;
		using ComponentCount = std::size_t;
		using ComponentIndex = std::size_t;
		using ComponentVector = std::vector<char>;
	public:
		ComponentManager() {};
		~ComponentManager() {};

		void SetWorldPointerToThis()
		{
			Impl::SimpleWorldComponentManager::SetComponentManager(this);
		};

		template<typename ComponentType>
		inline ComponentIndex AddComponent(const ComponentType& aComponent)
		{
			const ComponentID componentID = typeid(ComponentType);
			ComponentVector& component = myComponents[componentID].second;

			auto it = myComponents.find(componentID);

			if (it != myComponents.end())
			{
				ComponentCount& count = myComponents[componentID].first;
		
				std::vector<ComponentIndex>& slots = myAvaliableSlot[componentID];

				if (slots.size() > 0)
				{
					const ComponentIndex avaliableIndex = slots.back();
					slots.pop_back();

					std::memcpy(&component[avaliableIndex * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
					count++;

					return avaliableIndex;
				}

				if (component.size() <= count * sizeof(ComponentType))
				{
					component.resize((count * 2) * sizeof(ComponentType));
				}

				std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
				++count;
			}
			else
			{
				ComponentCount& count = myComponents[componentID].first;

				if (component.size() <= count * sizeof(ComponentType))
				{
					component.resize((count + 1) * sizeof(ComponentType));
				}

				std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
				++count;
			}

			const ComponentCount& count = myComponents[componentID].first - 1;
			return count;
		}

		template<typename ComponentType>
		inline void RemoveComponent(const ComponentIndex aIndex)
		{
			const ComponentID typeIndex = typeid(ComponentType);

			ComponentCount& count = myComponents[typeIndex].first;
			ComponentVector& components = myComponents[typeIndex].second;

			ComponentType* componentPointer = reinterpret_cast<ComponentType*>(&components[aIndex * sizeof(ComponentType)]);
			componentPointer->~ComponentType();

			--count;

			myComponents[typeid(ComponentType)].push_back(aIndex);
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
			const ComponentID typeIndex = typeid(ComponentType);

			ComponentVector& componentBytes = myComponents[typeIndex].second;

			const ComponentCount componentCount = myComponents[typeIndex].first;
			const std::size_t elementCount = componentBytes.size() / sizeof(ComponentType);

			if (elementCount != componentCount) //I still don't know why they wouldn't be same size will study further in future
			{
				componentBytes.resize(componentCount * sizeof(ComponentType));
			}

			return reinterpret_cast<std::vector<ComponentType>&>(componentBytes);
		}
	private:
		std::unordered_map<ComponentID, std::pair<ComponentCount, ComponentVector>> myComponents;
		std::unordered_map<ComponentID, std::vector<ComponentIndex>> myAvaliableSlot;
	};
}

namespace Simple
{
	class Entity final
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
				return World::GetComponentManager()->GetComponent<ComponentType>(it->second);
			}
			else
			{
				return nullptr;
			}
		}

		template<typename ComponentType>
		inline std::string GetComponentNameAsString()
		{
			std::string componentName;

			const auto it = myComponents.find(typeid(ComponentType));

			if (it != myComponents.end())
			{
				const std::string fullName = typeid(ComponentType).name();
				const std::size_t startPos = fullName.find(" ") + 1;
				const std::size_t endPos = fullName.find(" ", startPos);

				componentName = fullName.substr(startPos, endPos - startPos);

				const std::size_t namespacePos = componentName.find("::");

				if (namespacePos != std::string::npos)
				{
					componentName = componentName.substr(namespacePos + 2);
				}
			}

			return componentName;
		}

		template<typename ComponentType>
		inline void RemoveComponent()
		{
			auto it = myComponents.find(typeid(ComponentType));

			if (it != myComponents.end())
			{
				World::GetComponentManager()->RemoveComponent<ComponentType>(it->second);
				myComponents.erase(it);
			}
		}
	private:
		std::unordered_map<ComponentID, ComponentIndex> myComponents;
	};
}

namespace Simple
{
	class System
	{
	public:
		System() {};
		virtual ~System() {};
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void EarlyUpdate() {};
		virtual void FixedUpdate() {};
		virtual void LateUpdate() {};
	};

	class SystemManager final
	{
	public:
		inline void AddSystem(std::unique_ptr<System> aSystem)
		{
			mySystems.push_back(std::move(aSystem));
			mySystemPointer[typeid(*mySystems.back())] = mySystems.back().get();
		}

		template<typename SystemType>
		inline SystemType* GetSystem()
		{
			auto it = mySystemPointer.find(typeid(SystemType));

			if (it != mySystemPointer.end())
			{
				return dynamic_cast<SystemType*>(it->second);
			}
			else
			{
				return nullptr;
			}
		}

		inline void Update()
		{
			for (auto& system : mySystems)
			{
				system->EarlyUpdate();
				system->FixedUpdate();
				system->Update();
				system->LateUpdate();
			}
		}

		inline void Render()
		{
			for (auto& system : mySystems)
			{
				system->Render();
			}
		}
	private:
		std::vector<std::unique_ptr<System>> mySystems;
		std::unordered_map<std::type_index, System*> mySystemPointer;
	};
}

struct HelloWorld
{
	int a = 5;
};

namespace Simple
{
	class PlayerSystem final : public System
	{
	public:
		void Update() override
		{
			for (std::size_t i = 0; i < myEntities.size(); ++i)
			{
				std::cout << myEntities[i].GetComponent<HelloWorld>()->a << std::endl;
			}
		};

		void Render() override {};

		std::vector<Simple::Entity> myEntities;
	};
}

//Simple::ComponentManager componentManager;
//Simple::SystemManager systemManager;
//componentManager.SetWorldPointerToThis();
//systemManager.AddSystem(std::move(std::make_unique<Simple::PlayerSystem>()));
//auto playerSystem = systemManager.GetSystem<Simple::PlayerSystem>();
//
//for (size_t i = 0; i < 10; ++i)
//{
//	HelloWorld hello;
//	hello.a = static_cast<int>(i);
//
//	Simple::Entity entity;
//	entity.AddComponent(hello);
//
//	playerSystem->myEntities.push_back(entity);
//}
//
//systemManager.Update();