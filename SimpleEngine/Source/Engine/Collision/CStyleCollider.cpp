#pragma once
#include "Engine/Precomplied/EnginePch.hpp"
#include "CStyleCollider.hpp"
#include "CollisionFunctions.hpp"

namespace Simple
{

	void* CreateShape(eShapeType aShapeType)
	{
		switch (aShapeType)
		{
		case eShapeType::Sphere:
			return new Sphere();
			break;
		case eShapeType::AABB:
			return new AABB3D();
			break;
		case eShapeType::Ray:
			return new Ray();
			break;
		default:
			break;
		}

		return nullptr;
	}



	void DestroyShape(ShapeRepresentation& aShapeRepresentation)
	{
		switch (aShapeRepresentation.myShapeType)
		{
		case eShapeType::Sphere:
			delete aShapeRepresentation.GetShape<eShapeType::Sphere>();
			break;
		case eShapeType::AABB:
			delete aShapeRepresentation.GetShape<eShapeType::AABB>();
			break;
		case eShapeType::Ray:
			delete aShapeRepresentation.GetShape<eShapeType::Ray>();
			break;
		default:
			break;
		}
	}

	ShapeRepresentation::ShapeRepresentation(eShapeType aShapeType)
		: myShapePtr(CreateShape(aShapeType))
		, myShapeType(aShapeType)
	{
	}

	ShapeRepresentation::ShapeRepresentation(const ShapeRepresentation& aOther)
		: myShapePtr(CreateShape(aOther.myShapeType))
		, myShapeType(aOther.myShapeType)
	{
	}

	ShapeRepresentation::ShapeRepresentation(ShapeRepresentation&& aOther) noexcept
		: myShapePtr(std::exchange(aOther.myShapePtr, nullptr))
		, myShapeType(std::exchange(aOther.myShapeType, eShapeType::Sphere))
	{
	}

	ShapeRepresentation& ShapeRepresentation::operator=(const ShapeRepresentation& aOther)
	{
		DestroyShape(*this);

		myShapePtr = CreateShape(aOther.myShapeType);
		myShapeType = aOther.myShapeType;

		return *this;
	}

	ShapeRepresentation& ShapeRepresentation::operator=(ShapeRepresentation&& aOther) noexcept
	{
		myShapePtr = std::exchange(aOther.myShapePtr, nullptr);
		myShapeType = std::exchange(aOther.myShapeType, eShapeType::Sphere);

		return *this;
	}

	ShapeRepresentation::~ShapeRepresentation()
	{
		DestroyShape(*this);
	}


	CStyle_Collider::CStyle_Collider(eShapeType aShapeType)
		: myShape(aShapeType)
	{
	}


	template<typename T>
	bool ResolveShape(const T& aShape1, const ShapeRepresentation& aOtherShape)
	{
		switch (aOtherShape.myShapeType)
		{
		case eShapeType::Sphere:
		{
			return DetectCollision(aShape1, *aOtherShape.GetShape<eShapeType::Sphere>());
			break;
		}
		case eShapeType::AABB:
		{
			return DetectCollision(aShape1, *aOtherShape.GetShape<eShapeType::AABB>());
			break;
		}

		case eShapeType::Ray:
		{
			return DetectCollision(aShape1, *aOtherShape.GetShape<eShapeType::Ray>());
			break;
		}
		default:
			break;
		}

		return false;
	}

	bool CStyle_Collider::DoesCollideWith(const CStyle_Collider& aOtherCollider) const
	{
		switch (myShape.myShapeType)
		{
		case eShapeType::Sphere:
		{
			return ResolveShape(*myShape.GetShape<eShapeType::Sphere>(), aOtherCollider.myShape);
			break;
		}
		case eShapeType::AABB:
		{
			return ResolveShape(*myShape.GetShape<eShapeType::AABB>(), aOtherCollider.myShape);
			break;
		}
		case eShapeType::Ray:
		{
			return ResolveShape(*myShape.GetShape<eShapeType::Ray>(), aOtherCollider.myShape);
			break;
		}
		default:
			break;
		}

		return true;
	}
}