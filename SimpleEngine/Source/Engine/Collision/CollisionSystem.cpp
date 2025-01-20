#include "Engine/Precomplied/EnginePch.hpp"
#include "CollisionSystem.hpp"
#include "CStyleCollider.hpp"
#include "CollisionSystem.hpp"
#include "CollisionSystem.hpp"

namespace Simple
{
	CollisionSystem::CollisionSystem(const CollisionSystem& aOther)
	{
		for (size_t i = 0; i < aOther.myColliders.size(); i++)
		{
			myColliders.emplace_back(std::make_unique<Collider>(*aOther.myColliders[i]));
		}
	}

	void CollisionSystem::ProcessCollisions()
	{

		for (auto& collider : myColliders)
		{
			collider->UpdateFrameCollisions();
		}

		std::vector<std::pair<Collider*, Collider*>> detectedCollisions;

		for (int i = 0; i < static_cast<int>(myColliders.size()) - 1; i++)
		{
			for (int j = i + 1; j < myColliders.size(); j++)
			{
				Collider* collider1 = myColliders[i].get();
				Collider* collider2 = myColliders[j].get();
				if ((collider1->myOwnCollisionLayer & collider2->myCollidesWithLayer) == eCollisionLayer::None &&
					(collider2->myOwnCollisionLayer & collider1->myCollidesWithLayer) == eCollisionLayer::None)
				{
					continue;
				}


				if (collider1->DoesCollideWith(collider2->myShape))
				{
					detectedCollisions.push_back({ collider1, collider2 });
					collider1->myCurrentFrameCollisions.push_back(collider2);
					collider2->myCurrentFrameCollisions.push_back(collider1);
				}
			}
		}


		for (const auto& collision : detectedCollisions)
		{
			Collider* collider1 = collision.first;
			Collider* collider2 = collision.second;
			if (!collision.first->DidCollidePreviousFrame(*collider2))
			{
				if (collider1->myOnEnterCallback)
				{
					collider1->myOnEnterCallback(collider2);
				}
				if (collider2->myOnEnterCallback)
				{
					collider2->myOnEnterCallback(collider1);
				}
			}

			if (collider1->myOnOverlapCallback)
			{
				collider1->myOnOverlapCallback(collider2);
			}
			if (collider2->myOnOverlapCallback)
			{
				collider2->myOnOverlapCallback(collider1);
			}
		}

		for (const auto& collision : myPreviousFrameCollisions)
		{
			Collider* collider1 = collision.first;
			Collider* collider2 = collision.second;
			if (!collider1->DidCollideCurrentFrame(*collider2))
			{
				if (collider1->myOnExitCallback)
				{
					collider1->myOnExitCallback(collider2);
				}
				if (collider2->myOnExitCallback)
				{
					collider2->myOnExitCallback(collider1);
				}
			}
		}

		myPreviousFrameCollisions = detectedCollisions;
	}


	std::vector<Collider*> CollisionSystem::PollCollisions(const Shape& aShape) const
	{
		std::vector<Collider*> colliders;
		for (auto& collider : myColliders)
		{
			if (collider->DoesCollideWith(aShape))
			{
				colliders.push_back(collider.get());
			}
		}

		return colliders;
	}

	void CollisionSystem::RenderColliders()
	{
		for (const auto& collider : myColliders)
		{
			collider->Render();
		}
	}
}