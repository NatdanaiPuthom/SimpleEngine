#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"

namespace ECS
{
	AnimationSystem::AnimationSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	AnimationSystem::~AnimationSystem()
	{
	}

	void AnimationSystem::Update()
	{
		const std::unordered_set<EntityID>& entityIDs = myEntityComponentSystem->GetEntityIDsWithThisComponent<ECS::AnimationComponent>();

		for (auto& id : entityIDs)
		{
			ECS::Entity entity = myEntityComponentSystem->GetEntity(id);

			if (entity != nullptr)
			{
				ECS::AnimationComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationComponent>();

				if (animationPlayerComponent != nullptr)
				{
					animationPlayerComponent->animationPlayer.UpdateTest(animationPlayerComponent->jointMatrices, animationPlayerComponent);
				}
			}
		}
	}

	std::unique_ptr<System> AnimationSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<AnimationSystem>(aEntityComponentSystem);
	}
}