#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	struct TransformComponent;
	struct RewindTestComponent;

	class RewindSystem final : public ECS::System
	{
	public:
		 RewindSystem();
		~RewindSystem() override;

		virtual std::unique_ptr<System> Clone() const override;

		void Init() override;
		void Update(EntityComponentSystem* aEntityComponentSystem) override;

		void GoToPoint(ECS::TransformComponent* aTransformComponent, ECS::RewindTestComponent* aRewindTestComponent);
	};
}