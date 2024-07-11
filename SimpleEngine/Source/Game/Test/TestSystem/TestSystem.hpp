#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class EntityComponentSystem;

	class TestSystem : public ECS::System
	{
	public:
		TestSystem(EntityComponentSystem* aEntityComponentSystem);
		~TestSystem() override;

		void Init() override;
		void Update() override;
		void Render() override;

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const
		{
			return std::make_unique<TestSystem>(aEntityComponentSystem);
		}
	};
}