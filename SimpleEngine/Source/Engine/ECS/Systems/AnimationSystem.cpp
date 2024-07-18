#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"

namespace ECS
{
	AnimationSystem::AnimationSystem()
	{
	}

	AnimationSystem::~AnimationSystem()
	{
	}

	void AnimationSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& entityIDs = aEntityComponentSystem->GetEntityIDsWithThisComponent<ECS::AnimationComponent>();

		for (auto& id : entityIDs)
		{
			ECS::IEntity& entity = aEntityComponentSystem->GetEntity(id);
			ECS::AnimationComponent* animationPlayerComponent = entity.GetComponent<ECS::AnimationComponent>();

			if (animationPlayerComponent != nullptr && animationPlayerComponent->animation != nullptr)
			{
				animationPlayerComponent->animationPlayer.UpdateTest(animationPlayerComponent);
			}
		}
	}

	std::unique_ptr<System> AnimationSystem::Clone() const
	{
		return std::make_unique<AnimationSystem>();
	}
}