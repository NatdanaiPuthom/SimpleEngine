#pragma once
#include "Engine/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"

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

		Entity CreateEntity();
		Entity GetEntity(const EntityID aID);
		Entities GetAllEntities();

	public:
		void SetGlobalPointerToThis();
	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
	};
}