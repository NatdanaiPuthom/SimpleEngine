#pragma once
#include "Engine/Math/Transform.hpp"

namespace SimpleUtilities
{
	class InputManager;
}

namespace Simple
{
	class Camera final
	{
	public:
		Camera(const float aFoV = 1.57075f, const float aNearPlane = 0.1f, const float aFarPlane = 10000.0f);
		~Camera() = default;

		void Init();

		void Update(const float aDeltaTime);
		void UpdateResolution();

		Math::Vector4f WorldToCameraSpace(const Math::Vector4f& aVector) const;
		Math::Vector4f CameraToProjectionSpace(const Math::Vector4f& aVector) const;
		Math::Vector2f ProjectionToPixel(const Math::Vector4f& aVector) const;
	public:
		void SetCameraValues(const Math::Vector3f& aPosition, const float aNearPlane, const float aFoV);
		void SetPosition(const Math::Vector3f& aPosition);
		void SetRotation(const Math::Vector3f aRotationInDegree);
		void SetNearPlane(const float aNearPlane);
		void SetFoV(const float aFoV);

		void SetMoveSpeed(const float aSpeed);
		void SetRotateSpeed(const float aRotationSpeed);
	public:
		Math::Matrix4x4f GetWorldToClipMatrix();
		Math::Matrix4x4f GetModelToWorldMatrix() const;
		Math::Matrix4x4f GetProjectionMatrix() const;
		Math::Matrix4x4f GetViewMatrix() const;

		Math::Vector3f GetPosition() const;
		Math::Vector3f GetRotation() const;
		Math::Vector3f GetForward() const;
		Math::Vector3f GetUp() const;
		Math::Vector3f GetRight() const;

		float GetRotationSpeed() const;
		float GetMoveSpeed() const;
		float GetNearPlane() const;
		float GetFarPlane() const;
		float GetFoV() const;
	private:
		void CreateProjectionMatrix();
		void UpdateCameraVectors();
	private:
		SimpleUtilities::InputManager* myInput;

		Math::Matrix4x4f myProjectionMatrix;
		Math::Transform myTransform;

		Math::Vector3f myForward;
		Math::Vector3f myRight;
		Math::Vector3f myUp;

		Math::Vector2i myCapturedPosition;

		float myFoV;
		float myNearPlane;
		float myFarPlane;
		float myMoveSpeed;
		float myRotateSpeed;

		bool myFreeFly;
	};
}