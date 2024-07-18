#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class EntityComponentSystem;

	class AnimationSystem : public ECS::System
	{
	public:
		 AnimationSystem();
		~AnimationSystem() override;

		void Update(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	};
}