#include "Game/Precomplied/GamePch.hpp"
#include "RealEnemySystem.hpp"
#include "RealEnemyComponent.hpp"

namespace ECS
{

	RealEnemySystem::RealEnemySystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	std::unique_ptr<System> RealEnemySystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RealEnemySystem>(aEntityComponentSystem);
	}

	RealEnemySystem::~RealEnemySystem()
	{
	}

	void RealEnemySystem::Init()
	{
	}

	void RealEnemySystem::Update()
	{
		const auto& enemies = myEntityComponentSystem->GetEntityIDsWithThisComponent<RealEnemyComponent>();
		for (auto& enemy : enemies)
		{
			Entity entity = myEntityComponentSystem->GetEntity(enemy);
			auto transform = entity->GetComponent<TransformComponent>();
			auto mesh = entity->GetComponent<MeshComponent>();
			auto pos = transform->transform.GetPosition();
			pos.x += 10 * Global::GetDeltaTime();
			transform->transform.SetPosition(pos);
			transform->transform.LookAt({ 0,10,0 });

			if (MainSingleton::GetInputManager().IsKeyPressed('A'))
			{
				mesh->mesh = Global::GetModelFactory()->LoadMesh("Assets\\Models\\StaticModels\\SM_Particle_Chest.fbx");
			}

			auto entities = MainSingleton::GetSceneManager().GetCurrentECS().GetAllEntities();
			//MainSingleton::GetInputManager().IsKeyPressed('A');
			for (int i = 0; i < entities.GetEntityCount(); i++)
			{
				std::cout << entities[i]->GetName() << std::endl;
			}

		}
	}

}