#pragma once
#include "Engine/ECS/Core/System.hpp"
#include "Collision/CollisionSystem.hpp"

namespace ECS
{

	class ECSCollisionSystem : public ECS::System
	{
	public:
		ECSCollisionSystem();

		void Init(EntityComponentSystem*) override;
		void Update(EntityComponentSystem*) override;
		void Render(EntityComponentSystem*) override;

		std::unique_ptr<System> Clone() const override;

	private:

		std::vector<std::pair<EntityID, EntityID>> myPreviousFrameCollisions;
	};
}