#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class EntityComponentSystem;

	class AnimationSystem : public ECS::System
	{
	public:
		explicit AnimationSystem(EntityComponentSystem* aEntityComponentSystem);
		~AnimationSystem() override;

		void Update() override;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;
	};
}