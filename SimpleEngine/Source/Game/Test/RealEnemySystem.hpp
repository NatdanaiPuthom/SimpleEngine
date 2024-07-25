#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{

	class RealEnemySystem final : public ECS::System
	{
	public:
		RealEnemySystem();

		virtual std::unique_ptr<System> Clone() const override;

		~RealEnemySystem() override;

		void Update(EntityComponentSystem* aEntityComponentSystem) override;
		/*
		void Render() override;

		void EarlyUpdate() override;
		void FixedUpdate() override;
		void LateUpdate() override;*/
	};
}