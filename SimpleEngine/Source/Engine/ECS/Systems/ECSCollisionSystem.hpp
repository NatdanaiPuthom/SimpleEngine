#pragma once
#include "Engine/ECS/Core/System.hpp"
#include "Collision/CollisionSystem.hpp"

namespace ECS
{

	class ECSCollisionSystem : public ECS::System
	{
	public:
		ECSCollisionSystem();
		~ECSCollisionSystem();

		void Init(EntityComponentSystem*) override;
		void Update(EntityComponentSystem*) override;
		void Render(EntityComponentSystem*) override;

		std::unique_ptr<System> Clone() const override;

		void OnKnifePickup(Simple::Collider*);
		void OnKnifeHovered(Simple::Collider*);

	private:
		
		Simple::Collider* myKnifeCollider = nullptr;
		Simple::Collider* myEnemyCollider = nullptr;
		Simple::Collider* myRayCollider = nullptr;

		Simple::CollisionSystem myCollisionSystem;
	};
}