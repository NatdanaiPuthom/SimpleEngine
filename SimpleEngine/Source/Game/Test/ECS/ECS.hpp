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

		template<typename T>
		std::vector<T*> GetAllComponentsOfType();

	private:
		ComponentManager myComponentManager;
		SystemManager mySystemManager;
		EntityManager myEntityManager;
	};

	template<typename T>
	inline std::vector<T*> ECS::GetAllComponentsOfType()
	{
		return myComponentManager.GetAllComponentsOfType<T>();
	}
}