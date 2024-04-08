#pragma once
#include "Game/Test/ECS/ComponentManager.hpp"
#include "Game/Test/ECS/SystemManager.hpp"
#include "Game/Test/ECS/EntityManager.hpp"

namespace Simple
{
	class ECS
	{
	public:
		ECS();
		~ECS();

		void Init();
		void Update();
		void Render();

		Entity& CreateEntity();

	public:
		size_t GetEntityID(Entity* aEntity);
		Entity* GetEntity(const size_t aEntityID) const;
		std::vector<Entity>& GetAllEntities();
		const std::vector<Entity>& GetAllEntities() const;

	private:
		Simple::ComponentManager myComponentManager;
		Simple::SystemManager mySystemManager;
		Simple::EntityManager myEntityManager;
	};
}