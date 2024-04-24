#pragma once
#include "Game/Test/ECS/Core/ComponentManager.hpp"
#include "Game/Test/ECS/Core/SystemManager.hpp"
#include "Game/Test/ECS/Core/EntityManager.hpp"

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

		Entity CreateEntity();

		void SetGlobalECSPointerToThis();

	public:
		EntityClass* GetEntity(const size_t aEntityID) const;

		Simple::ComponentManager* GetComponentManager();

		std::vector<EntityClass*> GetAllEntities();
		const std::vector<EntityClass*> GetAllEntities() const;

		template<typename T>
		std::vector<T*> GetAllComponentsOfType();

	private:
		Simple::ComponentManager myComponentManager;
		Simple::SystemManager mySystemManager;
		Simple::EntityManager myEntityManager;
	};

	template<typename T>
	inline std::vector<T*> ECS::GetAllComponentsOfType()
	{
		return myComponentManager.GetAllComponentsOfType<T>();
	}
}