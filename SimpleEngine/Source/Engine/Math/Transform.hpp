#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Quaternion.hpp"

namespace Math
{
	class Transform final
	{
	public:
		Transform();
		Transform(const Vector3f& aPosition, const Vector3f& aRotation, const Vector3f& aScale);
		~Transform();

		void LookAt(const Vector3f& aTargetPoint);
		void SetPosition(const Vector3f& aPosition);
		void SetRotation(const Vector3f& aRotationInDegree);
		void SetScale(const Vector3f& aScale);
		void SetScale(const float aScale);

		Matrix4x4f GetMatrix() const; 

		Vector3f GetPosition() const;
		Vector3f GetRotation() const;
		Vector3f GetScale() const;

	private:
		Matrix4x4f myMatrix;
		Vector3f myPosition;
		Vector3f myRotation;
		Vector3f myScale;
	};

	inline Transform::Transform()
		: myScale(1,1,1)
	{
	}

	inline Transform::Transform(const Vector3f& aPosition, const Vector3f& aRotation, const Vector3f& aScale)
		: myPosition(aPosition)
		, myRotation(aRotation)
		, myScale(aScale)
	{
		myMatrix.SetPosition(myPosition);
		myMatrix.SetWorldRotation(myRotation);
		myMatrix.SetScale(myScale);
	}

	inline Transform::~Transform()
	{
	}

	inline void Transform::LookAt(const Vector3f& aTargetPoint)
	{
		myMatrix.LookAt(aTargetPoint);
	}

	inline void Transform::SetPosition(const Vector3f& aPosition)
	{
		myPosition = aPosition;
		myMatrix.SetPosition(myPosition);
	}

	inline void Transform::SetRotation(const Vector3f& aRotationInDegree)
	{
		myRotation = aRotationInDegree;

		if (myRotation.x < 0.0f) myRotation.x += 360.0f;
		else if (myRotation.x > 360.0f) myRotation.x -= 360.0f;

		if (myRotation.y < 0.0f) myRotation.y += 360.0f;
		else if (myRotation.y > 360.0f) myRotation.y -= 360.0f;

		if (myRotation.z < 0.0f) myRotation.z += 360.0f;
		else if (myRotation.z > 360.0f) myRotation.z -= 360.0f;

		myMatrix.SetWorldRotation(myRotation);
	}

	inline void Transform::SetScale(const Vector3f& aScale)
	{
		myScale = aScale;
		myMatrix.SetScale(myScale);
	}

	inline void Transform::SetScale(const float aScale)
	{
		myScale = {aScale, aScale, aScale};
		myMatrix.SetScale(myScale);
	}

	inline Vector3f Transform::GetPosition() const
	{
		return myPosition;
	}

	inline Vector3f Transform::GetRotation() const
	{
		return myRotation;
	}

	inline Vector3f Transform::GetScale() const
	{
		return myScale;
	}

	inline Matrix4x4f Transform::GetMatrix() const
	{
		return myMatrix;
	}
}