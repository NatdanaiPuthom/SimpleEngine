#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	struct TransformComponent;
	struct RewindTestComponent;

	class RewindSystem final : public ECS::System
	{
	public:
		explicit RewindSystem(EntityComponentSystem* aEntityComponentSystem);
		~RewindSystem() override;

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;

		void Init() override;
		void Update() override;

		void GoToPoint(ECS::TransformComponent* aTransformComponent, ECS::RewindTestComponent* aRewindTestComponent);
	};
}