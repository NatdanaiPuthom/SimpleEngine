#pragma once
#include "Engine/SimpleUtilities/Sphere.hpp"
#include "Engine/SimpleUtilities/AABB3D.hpp"
#include "Engine/SimpleUtilities/Ray.hpp"
#include "Engine/SimpleUtilities/Color.hpp"
#include "Engine/Collision/CollisionLayer.hpp"
#include <variant>
#include <functional>

namespace Simple
{
	using Shape = std::variant<Sphere, AABB3D, Ray>;

	class Collider
	{
		using CollisionCallback = std::function<void(Collider* aCollider)>;
		friend class CollisionSystem;
	public:

		Collider() = default;

		explicit Collider(const Shape& aShape, eCollisionLayer aOwnCollisionLayer, eCollisionLayer aCollidesWithLayer)
			: myShape(aShape)
			, myOwnCollisionLayer(aOwnCollisionLayer)
			, myCollidesWithLayer(aCollidesWithLayer)
		{
		}

		template<typename ShapeType>
		decltype(auto) GetShape()
		{
			return std::get<ShapeType>(myShape);
		}

		bool DoesCollideWith(const Shape& aShape) const;

		void SetOnEnterCallback(CollisionCallback aCallback)
		{
			myOnEnterCallback = std::move(aCallback);
		}

		void SetOnOverlapCallback(CollisionCallback aCallback)
		{
			 myOnOverlapCallback = std::move(aCallback);
		}

		void SetOnExitCallback(CollisionCallback aCallback)
		{
			myOnExitCallback = std::move(aCallback);
		}

		void SetColor(const Color& aColor)
		{
			myColor = aColor;
		}

		void Render() const;


		void UpdateFrameCollisions()
		{
			myPreviousFrameCollisions = myCurrentFrameCollisions;
			myCurrentFrameCollisions.clear();
		}


		bool DidCollidePreviousFrame(const Collider& aCollider) const
		{
			return std::find(begin(myPreviousFrameCollisions), end(myPreviousFrameCollisions), &aCollider) != end(myPreviousFrameCollisions);
		}

		bool DidCollideCurrentFrame(const Collider& aCollider) const
		{
			return std::find(begin(myCurrentFrameCollisions), end(myCurrentFrameCollisions), &aCollider) != end(myCurrentFrameCollisions);
		}

	public:

		Shape myShape;

		CollisionCallback myOnEnterCallback;
		CollisionCallback myOnOverlapCallback;
		CollisionCallback myOnExitCallback;

		std::vector<Collider*> myPreviousFrameCollisions;
		std::vector<Collider*> myCurrentFrameCollisions;

		Color myColor;

		eCollisionLayer myOwnCollisionLayer = eCollisionLayer::None;
		eCollisionLayer myCollidesWithLayer = eCollisionLayer::None;
	};
}