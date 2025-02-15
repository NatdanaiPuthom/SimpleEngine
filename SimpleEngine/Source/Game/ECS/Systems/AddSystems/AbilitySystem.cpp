#include "Game/Precomplied/GamePch.hpp"
#include "Game/ECS/Systems/AddSystems/AbilitySystem.hpp"
#include "Game/ECS/Components/AddComponents/AbilityComponent.hpp"
#include "Game/ECS/Components/AddComponents/StatsComponent.hpp"

static void FireAbility()
{
	std::cout << " casts Fireball!" << std::endl;
}

namespace ECS
{
	AbilitySystem::AbilitySystem()
	{
	}
	AbilitySystem::~AbilitySystem()
	{
	}
	void AbilitySystem::Init(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& statsEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<StatsComponent>();

		if (statsEntities.empty())
		{
			std::cout << "no entity with component 'StatsComponent'" << std::endl;
			return;
		}

		for (auto entityID : statsEntities)
		{
			/*Entity& entity = aEntityComponentSystem->GetEntity(entityID);*/

			//H key = 0x48
			//entity.AddComponent<AbilitComponent>(CreatAbility("Fire",40,0x48,FireAbility));

			std::vector<ECS::AbilitComponent> newAbilities;

			newAbilities.push_back(CreatAbility("Fire", 40, 0x48, FireAbility));

			EntitysAbilities[entityID] = newAbilities;
		}
	}
	void AbilitySystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		if (oneTime)
		{
			AbilitySystem::Init(aEntityComponentSystem);
			oneTime = false;
		}

		for (auto& [entityID, abilities] : EntitysAbilities)
		{
			Entity& entity = aEntityComponentSystem->GetEntity(entityID);
			StatsComponent* statsComponent = entity.GetComponent<StatsComponent>();

			for (AbilitComponent& ability : abilities)
			{
				if (GetAsyncKeyState(ability.key) & 0x0001)
				{
					if (statsComponent->currentMana >= ability.manaCost)
					{
						statsComponent->currentMana -= ability.manaCost;

						ability.execute();
					}
				}
			}
		}
	}
	void AbilitySystem::Render(EntityComponentSystem* /*aEntityComponentSystem*/)
	{

	}
	void AbilitySystem::EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void AbilitySystem::FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void AbilitySystem::LateUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	AbilitComponent AbilitySystem::CreatAbility(std::string aName, int aManaCost, int aKey/*, EntityID aEntityID*/, void (*aExecute)(void))
	{

		AbilitComponent newAbilityComponent;

		newAbilityComponent.name = aName;
		newAbilityComponent.manaCost = aManaCost;
		newAbilityComponent.key = aKey;
		newAbilityComponent.execute = aExecute;

		return newAbilityComponent;
	}

	std::unique_ptr<System> AbilitySystem::Clone() const
	{
		return std::make_unique<AbilitySystem>(*this);
	}
}

