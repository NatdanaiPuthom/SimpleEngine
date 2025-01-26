#pragma once
#include "SimpleUtilities/Sphere.hpp"
#include "SimpleUtilities/AABB3D.hpp"
#include "SimpleUtilities/Ray.hpp"
#include "CollisionFunctions.hpp"

namespace Simple
{
	template<typename T>
	class CollisionModel;

	class CollisionShapeOOP
	{
	public:

		CollisionShapeOOP() = default;
		virtual ~CollisionShapeOOP() = default;


		virtual bool TryCollision(const CollisionShapeOOP& aOther) const = 0;
		virtual bool TryCollision(const CollisionModel<Sphere>& aOther) const = 0;
		virtual bool TryCollision(const CollisionModel<AABB3D>& aOther) const = 0;
		virtual bool TryCollision(const CollisionModel<Ray>& aOther) const = 0;

	};

	template<typename ShapeType>
	class CollisionModel : public CollisionShapeOOP
	{
	public:

		CollisionModel(const ShapeType& aShape)
			: myShape(aShape)
		{
		}

		bool TryCollision(const CollisionShapeOOP& aOther) const override
		{
			return aOther.TryCollision(*this);
		}

		bool TryCollision(const CollisionModel<Sphere>& aOther) const override
		{
			return DetectCollision(myShape, aOther.myShape);
		}

		bool TryCollision(const CollisionModel<AABB3D>& aOther) const override
		{
			return DetectCollision(myShape, aOther.myShape);
		}

		bool TryCollision(const CollisionModel<Ray>& aOther) const override
		{
			return DetectCollision(myShape, aOther.myShape);
		}

	public:

		ShapeType myShape{};
	};


	class OOP_Collider
	{
	public:

		template<typename ShapeType>
		OOP_Collider(const ShapeType& aShape)
			: myCollisionShape(std::make_unique<CollisionModel<ShapeType>>(aShape))
		{

		}
		OOP_Collider(const OOP_Collider&) {}

		bool DoesCollideWith(const OOP_Collider& aOtherCollider) const;

	private:


		std::unique_ptr<CollisionShapeOOP> myCollisionShape;

	};
}