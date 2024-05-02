#pragma once
#include "Engine/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"

namespace Simple
{
	class ECS final
	{
	public:
		ECS();
		~ECS();

		ECS(const ECS&) = delete;
		ECS(ECS&&) = delete;
		ECS& operator=(const ECS&) = delete;
		ECS& operator=(ECS&&) = delete;

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