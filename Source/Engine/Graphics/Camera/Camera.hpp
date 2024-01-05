#pragma once

namespace Simple
{
	class Camera final
	{
	public:
		Camera(const float aFoV = 1.57075f, const float aNearPlane = 0.1f, const float aFarPlane = 10000.0f);
		~Camera() = default;

		void Update(const float aDeltaTime);
		void UpdateResolution();

		SimpleUtilities::Vector4f WorldToCameraSpace(const SimpleUtilities::Vector4f& aVector) const;
		SimpleUtilities::Vector4f CameraToProjectionSpace(const SimpleUtilities::Vector4f& aVector) const;
		SimpleUtilities::Vector2f ProjectionToPixel(const SimpleUtilities::Vector4f& aVector) const;
	public:
		void SetCameraValues(const SimpleUtilities::Vector3f& aPosition, const float aNearPlane, const float aFoV);
		void SetPosition(const SimpleUtilities::Vector3f& aPosition);
		void SetRotation(const SimpleUtilities::Vector3f aRotationInDegree);
		void SetNearPlane(const float aNearPlane);
		void SetFoV(const float aFoV);

		void SetMoveSpeed(const float aSpeed);
		void SetRotateSpeed(const float aRotationSpeed);
	public:
		SimpleUtilities::Matrix4x4f GetWorldToClipMatrix();
		SimpleUtilities::Matrix4x4f GetModelToWorldMatrix() const;
		SimpleUtilities::Matrix4x4f GetProjectionMatrix() const;
		SimpleUtilities::Matrix4x4f GetViewMatrix() const;

		SimpleUtilities::Vector3f GetPosition() const;
		SimpleUtilities::Vector3f GetRotation() const;
		SimpleUtilities::Vector3f GetForward() const;
		SimpleUtilities::Vector3f GetUp() const;
		SimpleUtilities::Vector3f GetRight() const;

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

		SimpleUtilities::Matrix4x4f myProjectionMatrix;
		SimpleUtilities::Transform myTransform;

		SimpleUtilities::Vector3f myForward;
		SimpleUtilities::Vector3f myRight;
		SimpleUtilities::Vector3f myUp;

		SimpleUtilities::Vector2i myCapturedPosition;

		float myFoV;
		float myNearPlane;
		float myFarPlane;
		float myMoveSpeed;
		float myRotateSpeed;

		bool myFreeFly;
	};
}