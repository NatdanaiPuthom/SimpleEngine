#include "Game/Precomplied/GamePch.hpp"
#include "RealEnemySystem.hpp"
#include "RealEnemyComponent.hpp"

namespace ECS
{
	RealEnemySystem::RealEnemySystem()
	{
	}

	std::unique_ptr<System> RealEnemySystem::Clone() const
	{
		return std::make_unique<RealEnemySystem>();
	}

	RealEnemySystem::~RealEnemySystem()
	{
	}

	void RealEnemySystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		const auto& enemies = aEntityComponentSystem->GetEntityIDsWithThisComponent<RealEnemyComponent>();

		for (auto& enemy : enemies)
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(enemy);
			auto transform = entity.GetComponent<TransformComponent>();
			auto mesh = entity.GetComponent<MeshComponent>();
			auto pos = transform->transform.GetPosition();
			pos.x += 10 * Global::GetDeltaTime();
			transform->transform.SetPosition(pos);
			transform->transform.LookAt({ 0,10,0 });

			if (MainSingleton::GetInputManager().IsKeyPressed('A'))
			{
				mesh->mesh = Global::GetModelFactory()->LoadMesh("Assets\\Models\\StaticModels\\SM_Particle_Chest.fbx");
			}

			const std::vector<Entity>& entities = MainSingleton::GetSceneManager().GetCurrentECS().GetAllEntities();
			
			for (int i = 0; i < entities.size(); i++)
			{
				std::cout << entities[i].GetName() << std::endl;
			}

		}
	}

}