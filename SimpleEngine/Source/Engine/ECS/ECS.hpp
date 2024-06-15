#pragma once
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
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

		EntityComponentSystem(const EntityComponentSystem&) = delete;
		EntityComponentSystem(EntityComponentSystem&&) = delete;
		EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;
		EntityComponentSystem& operator=(EntityComponentSystem&&) = delete;

		static void SaveData(EntityComponentSystem& aECS, const std::string& aFileName);
		static void LoadData(EntityComponentSystem& aECS, const std::string& aFileName);

		void Init();
		void Update();
		void Render();
		void RenderPointLights();
		void RenderSkyBoxAndDirectionalLight();

		void AddSystem(std::unique_ptr<System> aSystem);

		Entity CreateEntity(const EntityID aEntityID = 0);
		bool RemoveEntity(const EntityID aEntityID);
	public:

		Entity GetEntity(const EntityID aID);
		Entities GetAllEntities();
		void* GetComponentPointerByComponentID(const ComponentID aComponentID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();

	public:
		void SetGlobalPointerToThis();
	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
		SystemManager mySystemManager;
	};

	template<typename T>
	inline const std::unordered_set<EntityID>& EntityComponentSystem::GetEntityIDsWithThisComponent()
	{
		return myComponentManager.GetEntityIDsWithThisComponent<T>();
	}
}