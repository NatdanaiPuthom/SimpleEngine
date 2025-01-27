#pragma once
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/Core/ComponentManager.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include <memory>
#include <unordered_set>

namespace ECS
{
	class System;
}

namespace ECS
{
	class EntityComponentSystem final
	{
	public:
		EntityComponentSystem();
		~EntityComponentSystem();

		EntityComponentSystem(const EntityComponentSystem& aOther);
		EntityComponentSystem(EntityComponentSystem&& aOther) noexcept;
		EntityComponentSystem& operator=(const EntityComponentSystem& aOther);
		EntityComponentSystem& operator=(EntityComponentSystem&& aOther) noexcept;

		static void SaveData(EntityComponentSystem& aECS, const std::string& aFileName);
		static void LoadData(EntityComponentSystem& aECS, const std::string& aFileName);

		void Init();
		void Update();
		void Render();

		void EarlyUpdate();
		void FixedUpdate();
		void LateUpdate();
		void LateRender();
		void RenderEntitiesWithBloomComponent();

		template<typename T>
		void PrintMemoryPoolMemoryStateOfType() const;

	public:
		void AddClonedSystem(const size_t aSystemHashCode, std::unique_ptr<System> aSystem);
	public:
		ECS::Entity& CreateEntity(const EntityID aEntityID = 0);
	public:
		bool RemoveEntity(const EntityID aEntityID);
	public:
		EntityManager* GetEntityManager();
		ECS::Entity& GetEntity(const EntityID aID);
		std::vector<ECS::Entity>& GetAllEntities();
		void* GetComponentPointerByComponentID(const ComponentID aComponentID);

		template<typename T>
		std::vector<T*>* GetAllComponentsOfType();

		template<typename T>
		T* GetComponent(EntityID aEntityID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();
	private:
		static void EraseMissingElementFromJSON(const nlohmann::json& aJsonData, const std::string& aAbsolutePath, const size_t aEntityIndex, const size_t aComponentIndex);
		static void LoadComponentData(nlohmann::json& aPropertiesJSON, const std::vector<ComponentProperty>& aComponentProperties, const ComponentRegistry* aComponentRegistry, void* aComponentPointer);
	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
		SystemManager mySystemManager;
	};

	template<typename T>
	inline std::vector<T*>* EntityComponentSystem::GetAllComponentsOfType()
	{
		return myComponentManager.GetAllComponentsOfType<T>();
	}

	template<typename T>
	inline T* EntityComponentSystem::GetComponent(EntityID aEntityID)
	{
		return myEntityManager.GetComponent<T>(aEntityID);
	}

	template<typename T>
	inline const std::unordered_set<EntityID>& EntityComponentSystem::GetEntityIDsWithThisComponent()
	{
		return myComponentManager.GetEntityIDsWithThisComponent<T>();
	}

	template<typename T>
	inline void EntityComponentSystem::PrintMemoryPoolMemoryStateOfType() const
	{
		myComponentManager.PrintMemoryPoolMemoryStateOfType<T>();
	}
}