#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class AbilitySystem final : public ECS::System
	{
	public:
		AbilitySystem();
		~AbilitySystem() override;

		void Init(EntityComponentSystem* aEntityComponentSystem) override;
		void Update(EntityComponentSystem* aEntityComponentSystem) override;
		void Render(EntityComponentSystem* aEntityComponentSystem) override;

		void EarlyUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void FixedUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void LateUpdate(EntityComponentSystem* aEntityComponentSystem) override;

		virtual std::unique_ptr<System> Clone() const override;
	};
}