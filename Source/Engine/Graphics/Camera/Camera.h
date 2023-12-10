#pragma once

class Camera
{
public:
	Camera(const SimpleUtilities::Vector2f& aResolution = {1280, 720}, const float aFoV = 0.70f, const float aNearPlane = 0.1f, const float aFarPlane = 1000.0f);
	~Camera() = default;

	void Update(const float aDeltaTime);

	SimpleUtilities::Vector4f WorldToCameraSpace(const SimpleUtilities::Vector4f& aVector);
	SimpleUtilities::Vector4f CameraToProjectionSpace(const SimpleUtilities::Vector4f& aVector);
	SimpleUtilities::Vector2f ProjectionToPixel(const SimpleUtilities::Vector4f& aVector);
public:
	void SetCameraValues(const SimpleUtilities::Vector3f& aPosition, const SimpleUtilities::Vector2f& aResolution, const float aNearPlane, const float aFoV);
	void SetPosition(const SimpleUtilities::Vector3f& aPosition);
	void SetResolution(const SimpleUtilities::Vector2f& aResolution);
	void SetNearPlane(const float aNearPlane);
	void SetMoveSpeed(const float aSpeed);
	void SetFoV(const float aFoV);
public:
	SimpleUtilities::Matrix4x4f GetWorldToClipMatrix();
	SimpleUtilities::Matrix4x4f GetModelToWorldMatrix() const;
	SimpleUtilities::Matrix4x4f GetProjectionMatrix() const;
	SimpleUtilities::Matrix4x4f GetViewMatrix() const;
	SimpleUtilities::Vector3f GetPosition() const;
	SimpleUtilities::Vector3f GetRotationRad() const;
	SimpleUtilities::Vector3f GetForward() const;
	SimpleUtilities::Vector3f GetUp() const;
	SimpleUtilities::Vector3f GetRight() const;
	float GetMoveSpeed() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFoV() const;
private:
	void CreateProjectionMatrix();
	void UpdateCameraVectors();
private:
	SimpleUtilities::Matrix4x4f myModelToWorldTransform;
	SimpleUtilities::Matrix4x4f myProjectionMatrix;

	SimpleUtilities::Vector3f myCurrentPosition;
	SimpleUtilities::Vector3f myForward;
	SimpleUtilities::Vector3f myRight;
	SimpleUtilities::Vector3f myUp;

	SimpleUtilities::InputManager* myInput;
	SimpleUtilities::Vector2f myResolution;
	SimpleUtilities::Vector2i myCapturedPosition;

	float myFoV;
	float myNearPlane;
	float myFarPlane;
	float myMoveSpeed;
	float myRotateSpeed;

	bool myFreeFly;
};
