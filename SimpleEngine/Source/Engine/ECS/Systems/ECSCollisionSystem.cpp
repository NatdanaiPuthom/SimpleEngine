#include "Engine/Precomplied/EnginePch.hpp"
#include "ECSCollisionSystem.hpp"
#include "SimpleUtilities/DebugShapes.hpp"
#include "Engine/engine.hpp"
#include "Global.hpp"
#include "Collision/CollisionFunctions.hpp"
#include "Engine/ECS/Components/Core/ColliderComponent.hpp"

using namespace Simple;

namespace ECS
{
	ECSCollisionSystem::ECSCollisionSystem()
	{
	}

	void ECSCollisionSystem::Init(EntityComponentSystem*)
	{

	}

	void ECSCollisionSystem::Update(EntityComponentSystem* aECS)
	{
		auto& entityIDsSet = aECS->GetEntityIDsWithThisComponent<ColliderComponent>();

		std::vector<EntityID> entityIDs(begin(entityIDsSet), end(entityIDsSet));

		for (ECS::EntityID entityID : entityIDs)
		{
			ColliderComponent* colliderComponent = aECS->GetComponent<ColliderComponent>(entityID);
			ColliderComponentFunctions::UpdateFrameCollisions(*colliderComponent);
		}

		std::vector<std::pair<EntityID, EntityID>> detectedCollisions;

		for (int i = 0; i < static_cast<int>(entityIDs.size()) - 1; i++)
		{
			for (int j = i + 1; j < entityIDs.size(); j++)
			{
				ECS::EntityID entityID1 = entityIDs[i];
				ECS::EntityID entityID2 = entityIDs[i];
				ColliderComponent* colliderComponent1 = aECS->GetComponent<ColliderComponent>(entityID1);
				ColliderComponent* colliderComponent2 = aECS->GetComponent<ColliderComponent>(entityID2);
				if ((colliderComponent1->myOwnCollisionLayer & colliderComponent2->myCollidesWithLayer) == eCollisionLayer::None &&
					(colliderComponent2->myOwnCollisionLayer & colliderComponent1->myCollidesWithLayer) == eCollisionLayer::None)
				{
					continue;
				}

				if (ColliderComponentFunctions::DoesCollideWith(colliderComponent1->myShape, colliderComponent2->myShape))
				{
					detectedCollisions.push_back({ entityID1, entityID2 });
					colliderComponent1->myCurrentFrameCollisions.push_back(entityID2);
					colliderComponent2->myCurrentFrameCollisions.push_back(entityID1);
				}
			}
		}

		for (const auto& collision : detectedCollisions)
		{
			EntityID entityID1 = collision.first;
			EntityID entityID2 = collision.second;

			ColliderComponent* colliderComponent1 = aECS->GetComponent<ColliderComponent>(entityID1);
			ColliderComponent* colliderComponent2 = aECS->GetComponent<ColliderComponent>(entityID2);

			if (!ColliderComponentFunctions::DidCollidePreviousFrame(*colliderComponent1, entityID2))
			{
				if (colliderComponent1->myOnEnterCallback)
				{
					colliderComponent1->myOnEnterCallback(entityID2);
				}
				if (colliderComponent2->myOnEnterCallback)
				{
					colliderComponent2->myOnEnterCallback(entityID1);
				}
			}

			if (colliderComponent1->myOnOverlapCallback)
			{
				colliderComponent1->myOnOverlapCallback(entityID2);
			}
			if (colliderComponent2->myOnOverlapCallback)
			{
				colliderComponent2->myOnOverlapCallback(entityID1);
			}
		}

		for (const auto& collision : myPreviousFrameCollisions)
		{
			EntityID entityID1 = collision.first;
			EntityID entityID2 = collision.second;

			ColliderComponent* colliderComponent1 = aECS->GetComponent<ColliderComponent>(entityID1);
			ColliderComponent* colliderComponent2 = aECS->GetComponent<ColliderComponent>(entityID2);
			if (!ColliderComponentFunctions::DidCollideCurrentFrame(*colliderComponent1, entityID2))
			{
				if (colliderComponent1->myOnExitCallback)
				{
					colliderComponent1->myOnExitCallback(entityID2);
				}
				if (colliderComponent2->myOnExitCallback)
				{
					colliderComponent2->myOnExitCallback(entityID1);
				}
			}
		}

		myPreviousFrameCollisions = detectedCollisions;
	}

	void ECSCollisionSystem::Render(EntityComponentSystem* aECS)
	{
		auto& entityIDsSet = aECS->GetEntityIDsWithThisComponent<ColliderComponent>();

		for (auto& entityID : entityIDsSet)
		{
			ColliderComponent* colliderComponent = aECS->GetComponent<ColliderComponent>(entityID);
			ColliderComponentFunctions::Render(*colliderComponent);
		}
	}

	std::unique_ptr<System> ECSCollisionSystem::Clone() const
	{
		return std::make_unique<ECSCollisionSystem>(*this);
	}
}

