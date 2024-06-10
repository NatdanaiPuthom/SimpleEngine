#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"

namespace Simpleton
{
	class InputManager;
}

namespace Graphics
{
	enum class eCameraType
	{
		Perspective,
		Orthographic
	};

	class Camera final
	{
	public:
		Camera(const float aHorizontalFoVRad = 1.57075f, const float aNearPlane = 0.1f, const float aFarPlane = 10000.0f);
		~Camera() = default;

		void Init();

		void Update(const float aDeltaTime, const HWND& aHWND);
		void UpdateProjection(const Math::Vector2ui& aResolution);

		void InactiveFreeFly();
	public:
		void SetOrtographicProjection(const float aHalfSize, const float aNear, const float aFar);
		void SetPerspectiveProjection(const Math::Vector2ui& aResolution);

		void SetCameraType(const eCameraType aCameraType, const Math::Vector2ui& aResolution);

		void SetPosition(const Math::Vector3f& aPosition);
		void SetRotation(const Math::Vector3f& aRotationInDegree);
		void SetNearPlane(const float aNearPlane, const Math::Vector2ui& aResolution);
		void SetHorizontalFoV(const float aHorizontalFoVRad, const Math::Vector2ui& aResolution);

		void SetMoveSpeed(const float aSpeed);
		void SetRotateSpeed(const float aRotationSpeed);
	public:
		Math::Matrix4x4f GetWorldToClipMatrix() const;
		Math::Matrix4x4f GetMatrix() const;
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
		float GetHorizontalFoV() const;

		bool IsFreeFlyActive() const;

	private:
		Math::Transform myTransform;
		Math::Matrix4x4f myProjectionMatrix;
		Math::Vector2i myCapturedPosition;

		Simpleton::InputManager* myInput;

		float myHorizontalFoVRad;
		float myNearPlane;
		float myFarPlane;
		float myMoveSpeed;
		float myRotateSpeed;
		float myMouseTapTimer;

		eCameraType myCameraType;

		bool myFreeFly;
		bool myMouseIsTapped;
	};
}