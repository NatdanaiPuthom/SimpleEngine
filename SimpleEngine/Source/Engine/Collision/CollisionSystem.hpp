#pragma once
#include "Collider.hpp"
#include <vector>
#include <memory>


namespace Simple
{


	class CollisionSystem
	{
	public:


		CollisionSystem() = default;
		CollisionSystem(const CollisionSystem& aOther);



		void ProcessCollisions();

		void RenderColliders();


		std::vector<Collider*> PollCollisions(const Shape& aShape) const;

		Collider& CreateCollider(const Shape& aShape, eCollisionLayer aOwnCollisionLayer, eCollisionLayer aCollidesWithLayer)
		{
			return *myColliders.emplace_back(std::make_unique<Collider>(aShape, aOwnCollisionLayer, aCollidesWithLayer));
		}

	private:

		std::vector<std::unique_ptr<Collider>> myColliders;
		std::vector<std::pair<Collider*, Collider*>> myPreviousFrameCollisions;
	};

}