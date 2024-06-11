#pragma once
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include <memory>

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

		void Init();
		void Update();
		void Render();
		void RenderPointLights();
		void RenderSkyBoxAndDirectionalLight();

		void AddSystem(std::unique_ptr<System> aSystem);

		Entity CreateEntity();

		Entity GetEntity(const EntityID aID);
		Entities GetAllEntities();
		void* GetComponentPointerByComponentID(const ComponentID aComponentID);

	public:
		void SetGlobalPointerToThis();
	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
		SystemManager mySystemManager;
	};
}