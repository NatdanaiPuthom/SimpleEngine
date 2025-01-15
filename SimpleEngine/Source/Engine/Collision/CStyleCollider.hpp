#pragma once
#include "SimpleUtilities/Sphere.hpp"
#include "SimpleUtilities/AABB3D.hpp"
#include "SimpleUtilities/Ray.hpp"

namespace Simple
{
	enum class eShapeType
	{
		Sphere,
		AABB,
		Ray
	};

	class ShapeRepresentation
	{
	public:
		ShapeRepresentation(eShapeType aShapeType);
		ShapeRepresentation(const ShapeRepresentation& aOther);
		ShapeRepresentation(ShapeRepresentation&& aOther) noexcept;
		ShapeRepresentation& operator=(const ShapeRepresentation& aOther);
		ShapeRepresentation& operator=(ShapeRepresentation&& aOther) noexcept;
		~ShapeRepresentation();

		template<eShapeType ShapeType>
		auto GetShape() const
		{
			if constexpr (ShapeType == eShapeType::Sphere)
			{
				return static_cast<const Sphere*>(myShapePtr);
			}
			else if constexpr (ShapeType == eShapeType::AABB)
			{
				return static_cast<const AABB3D*>(myShapePtr);
			}
			else if constexpr (ShapeType == eShapeType::Ray)
			{
				return static_cast<const Ray*>(myShapePtr);
			}
			else
			{
				return nullptr;
			}
		}


		void* myShapePtr = nullptr;
		eShapeType myShapeType = eShapeType::Sphere;
	};


	class CStyle_Collider
	{
	public:

		CStyle_Collider() = default;
		CStyle_Collider(eShapeType aShapeType);

		bool DoesCollideWith(const CStyle_Collider& aOtherCollider) const;

	private:

		ShapeRepresentation myShape;

	};



}