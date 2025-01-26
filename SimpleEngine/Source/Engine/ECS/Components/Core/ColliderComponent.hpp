#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Collision/CollisionShape.hpp"
#include "Engine/Collision/CollisionLayer.hpp"
#include "Engine/SimpleUtilities/Color.hpp"
#include <variant>
#include <functional>
#include <vector>

#undef max

namespace Simple
{
	using CollisionCallback = std::function<void(ECS::ComponentID aComponentID)>;

	COMPONENT(ColliderComponent)
	{
		CollisionShape myShape;

		CollisionCallback myOnEnterCallback;
		CollisionCallback myOnOverlapCallback;
		CollisionCallback myOnExitCallback;

		std::vector<ECS::EntityID> myPreviousFrameCollisions;
		std::vector<ECS::EntityID> myCurrentFrameCollisions;

		ECS::EntityID myComponentID = std::numeric_limits<size_t>::max();

		Color myColor;

		eCollisionLayer myOwnCollisionLayer = eCollisionLayer::None;
		eCollisionLayer myCollidesWithLayer = eCollisionLayer::None;
	};

	REGISTER_DATATYPE(Sphere);

	REGISTER_AND_EXPOSE_PROPERTY(&Sphere::myCenter);
	REGISTER_AND_EXPOSE_PROPERTY(&Sphere::myRadius);

	REGISTER_DATATYPE(AABB3D);

	REGISTER_DATATYPE(Ray);

	REGISTER_DATATYPE(CollisionShape);
	REGISTER_AND_EXPOSE_PROPERTY(&ColliderComponent::myShape);

	class ColliderComponentFunctions
	{
	public:
		static bool DoesCollideWith(const ColliderComponent& aColliderComponent1, const CollisionShape& aShape);
		static bool DoesCollideWith(const ColliderComponent& aColliderComponent1, const ColliderComponent& aColliderComponent2);
		static bool DoesCollideWith(const CollisionShape& aShape1, const CollisionShape& aShape2);

		template<typename ShapeType>
		decltype(auto) GetShape(ColliderComponent& aColliderComponent)
		{
			return std::get<ShapeType>(aColliderComponent.myShape);
		}

		static void SetOnEnterCallback(ColliderComponent& aColliderComponent, CollisionCallback aCallback)
		{
			aColliderComponent.myOnEnterCallback = std::move(aCallback);
		}

		static void SetOnOverlapCallback(ColliderComponent& aColliderComponent, CollisionCallback aCallback)
		{
			aColliderComponent.myOnOverlapCallback = std::move(aCallback);
		}

		static void SetOnExitCallback(ColliderComponent& aColliderComponent, CollisionCallback aCallback)
		{
			aColliderComponent.myOnExitCallback = std::move(aCallback);
		}

		static void SetColor(ColliderComponent& aColliderComponent, const Simple::Color& aColor)
		{
			aColliderComponent.myColor = aColor;
		}

		static void Render(const ColliderComponent& aColliderComponent);

		static void UpdateFrameCollisions(ColliderComponent& aColliderComponent)
		{
			aColliderComponent.myPreviousFrameCollisions = aColliderComponent.myCurrentFrameCollisions;
			aColliderComponent.myCurrentFrameCollisions.clear();
		}

		static bool DidCollidePreviousFrame(ColliderComponent& aColliderComponent, const ECS::EntityID aEntityID)
		{
			return std::find(begin(aColliderComponent.myPreviousFrameCollisions), end(aColliderComponent.myPreviousFrameCollisions), aEntityID) != end(aColliderComponent.myPreviousFrameCollisions);
		}

		static bool DidCollideCurrentFrame(const ColliderComponent& aColliderComponent, const ECS::EntityID aEntityID)
		{
			return std::find(begin(aColliderComponent.myCurrentFrameCollisions), end(aColliderComponent.myCurrentFrameCollisions), aEntityID) != end(aColliderComponent.myCurrentFrameCollisions);
		}
	};

}