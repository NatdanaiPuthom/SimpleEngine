#include "Game/Precomplied/GamePch.hpp"
#include "Game/ECS/Systems/AddSystems/StatsSystem.hpp"
#include "Game/ECS/Components/AddComponents/StatsComponent.hpp"

namespace ECS
{
	StatsSystem::StatsSystem()
	{
	}
	StatsSystem::~StatsSystem()
	{
	}
	void StatsSystem::Init(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void StatsSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& statsEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<StatsComponent>();

		for (auto entityID : statsEntities)
		{
			Entity& entity = aEntityComponentSystem->GetEntity(entityID);
			StatsComponent* statsComponent = entity.GetComponent<StatsComponent>();

			if (statsComponent->currentMana < statsComponent->maxMana)
			{
				float deltaTime = Global::GetDeltaTime();

				statsComponent->currentMana += statsComponent->manaPerSec * deltaTime;

				if (statsComponent->currentMana >= statsComponent->maxMana)
				{
					statsComponent->currentMana = statsComponent->maxMana;
				}
			}
		}
	}
	void StatsSystem::Render(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& statsEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<StatsComponent>();

		for (auto entityID : statsEntities)
		{
			Entity& entity = aEntityComponentSystem->GetEntity(entityID);
			StatsComponent* statsComponent = entity.GetComponent<StatsComponent>();

			std::cout << "\nID: " + std::to_string(entityID) + " Mana: " + std::to_string(statsComponent->currentMana) << std::endl;
		}
	}
	void StatsSystem::EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void StatsSystem::FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void StatsSystem::LateUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	std::unique_ptr<System> StatsSystem::Clone() const
	{
		return std::make_unique<StatsSystem>(*this);
	}
}

