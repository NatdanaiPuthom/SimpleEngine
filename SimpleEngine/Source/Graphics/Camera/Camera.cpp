#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Graphics/Camera/Camera.hpp"

namespace Graphics
{
	Camera::Camera(const float aHorizontalFoVRad, const float aNearPlane, const float aFarPlane)
		: myHorizontalFoVRad(aHorizontalFoVRad)
		, myFarPlane(aFarPlane)
		, myNearPlane(aNearPlane)
		, myMoveSpeed(1.0f)
		, myRotateSpeed(45.0f)
		, myFreeFly(false)
		, myMouseIsTapped(false)
		, myInput(nullptr)
		, myMouseTapTimer(0.0f)
	{
		SetCameraType(eCameraType::Perspective, { 1280,720 });
	}

	void Camera::Init()
	{
		myInput = &MainSingleton::GetInputManager();
	}

	void Camera::Update(const float aDeltaTime, const HWND& aHWND)
	{
		if (GetForegroundWindow() != aHWND)
		{
			if (myInput->GetMouseIsHidden() == true)
			{
				myInput->ShowMouse();
				myInput->ReleaseMouse();
			}

			myFreeFly = false;
			myMouseIsTapped = false;
			return;
		}

		if (myInput->IsKeyPressed(VK_TAB) == true && myMouseIsTapped == false)
		{
			myFreeFly = !myFreeFly;
		}

		const bool tapped = myInput->IsKeyHeld(VK_RBUTTON);

		if (tapped == true)
		{
			myMouseTapTimer += aDeltaTime;

			if (myMouseTapTimer > 0.12f)
			{
				myMouseTapTimer = 0.0f;
				myMouseIsTapped = true;
			}
		}
		else
		{
			myMouseTapTimer = 0.0f;
			myMouseIsTapped = false;
		}

		if (myFreeFly == true || myMouseIsTapped == true)
		{
			if (myInput->GetMouseIsHidden() == false)
			{
				myInput->HideMouse();
				myInput->CaptureMouse();
			}

			const Math::Vector3f currentPosition = GetPosition();
			const Math::Vector3f currentRotation = GetRotation();

			const Math::Vector3f cameraForward = GetForward();
			const Math::Vector3f cameraRight = GetRight();
			const Math::Vector3f cameraUp = GetUp();

			Math::Vector3f forward;
			Math::Vector3f targetPosition(currentPosition);
			Math::Vector3f targetRotation(currentRotation);

			float speed = myMoveSpeed;

			if (myInput->IsKeyHeld(VK_SHIFT))
			{
				speed *= 5.0f;
			}

			const int mouseWheelDelta = myInput->GetMouseWheelDelta();
			myMoveSpeed += mouseWheelDelta;

			if (myMoveSpeed < 0)
			{
				myMoveSpeed = 0;
			}

			if (myInput->IsKeyHeld('W'))
			{
				forward = cameraForward * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('S'))
			{
				forward = -1.0f * cameraForward * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('A'))
			{
				forward = -1.0f * cameraRight * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('D'))
			{
				forward = cameraRight * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('Q'))
			{
				targetRotation.y -= myRotateSpeed * aDeltaTime;
			}

			if (myInput->IsKeyHeld('E'))
			{
				targetRotation.y += myRotateSpeed * aDeltaTime;
			}

			if (myInput->IsKeyHeld('Z'))
			{
				targetRotation.x -= myRotateSpeed * aDeltaTime;
			}

			if (myInput->IsKeyHeld('C'))
			{
				targetRotation.x += myRotateSpeed * aDeltaTime;
			}

			float direction = 1.0f;

			if (myInput->IsKeyHeld(VK_CONTROL))
			{
				direction = -1.0f;
			}

			if (myInput->IsKeyHeld(VK_SPACE))
			{
				targetPosition.y += direction * speed * cameraUp.y * aDeltaTime;
			}

			Math::Vector2f mouseDelta = myInput->GetMouseDelta();
			mouseDelta *= myRotateSpeed * 0.0015f;

			targetRotation.x += -mouseDelta.y;
			targetRotation.y += mouseDelta.x;

			SetRotation(targetRotation);
			SetPosition(targetPosition);
		}
		else
		{
			if (myInput->GetMouseIsHidden())
			{
				myInput->ShowMouse();
				myInput->ReleaseMouse();
			}
		}
	}

	Math::Matrix4x4f Camera::GetWorldToClipMatrix() const
	{
		const Math::Matrix4x4f clipMatrix = Math::Matrix4x4f::GetFastInverse(myTransform.GetMatrix()) * myProjectionMatrix;
		return clipMatrix;
	}

	void Camera::SetPerspectiveProjection(const Math::Vector2ui& aResolution)
	{
		const Math::Vector2f resolution(static_cast<float>(aResolution.x), static_cast<float>(aResolution.y));

		const float Q = myFarPlane / (myFarPlane - myNearPlane);
		const float verticalFoVRad = 2 * std::atan(std::tan(myHorizontalFoVRad / 2) * (resolution.y / resolution.x));
		const float scaleX = 1.0f / std::tanf(myHorizontalFoVRad / 2.0f);
		const float scaleY = 1.0f / std::tanf(verticalFoVRad * 0.5f);

		myProjectionMatrix(1, 1) = scaleX;
		myProjectionMatrix(2, 2) = scaleY;
		myProjectionMatrix(3, 3) = Q;
		myProjectionMatrix(3, 4) = 1.0f / Q;
		myProjectionMatrix(4, 3) = -Q * myNearPlane;
		myProjectionMatrix(4, 4) = 0.0f;
	}

	void Camera::SetOrtographicProjection(const float aHalfSize, const float aNear, const float aFar)
	{
		const float left = -aHalfSize;
		const float right = aHalfSize;
		const float top = -aHalfSize;
		const float bottom = aHalfSize;

		myProjectionMatrix = Math::Matrix4x4f::Identity();

		myProjectionMatrix(1, 1) = 2.0f / (right - left);
		myProjectionMatrix(2, 2) = 2.0f / (bottom - top);
		myProjectionMatrix(3, 3) = 1.0f / (aFar - aNear);

		myProjectionMatrix(4, 1) = -(right + left) / (right - left);
		myProjectionMatrix(4, 2) = -(bottom + top) / (bottom - top);
		myProjectionMatrix(4, 3) = -(aNear) / (aFar - aNear);
	}

	void Camera::SetPosition(const Math::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	void Camera::SetRotation(const Math::Vector3f& aRotationInDegree)
	{
		myTransform.SetRotation(aRotationInDegree);
	}

	void Camera::UpdateProjection(const Math::Vector2ui& aResolution)
	{
		switch (myCameraType)
		{
		case eCameraType::Perspective:
			SetPerspectiveProjection(aResolution);
			break;
		case eCameraType::Orthographic:
			break;
		}
	}

	void Camera::InactiveFreeFly()
	{
		myInput->ShowMouse();
		myInput->ReleaseMouse();
		myFreeFly = false;
	}

	void Camera::SetCameraType(const eCameraType aCameraType, const Math::Vector2ui& aResolution)
	{
		myCameraType = aCameraType;
		UpdateProjection(aResolution);
	}

	void Camera::SetNearPlane(const float aNearPlane, const Math::Vector2ui& aResolution)
	{
		myNearPlane = aNearPlane;

		if (myCameraType == eCameraType::Perspective)
		{
			SetPerspectiveProjection(aResolution);
		}
	}

	void Camera::SetMoveSpeed(const float aSpeed)
	{
		myMoveSpeed = aSpeed;
	}

	void Camera::SetRotateSpeed(const float aRotationSpeed)
	{
		myRotateSpeed = aRotationSpeed;
	}

	void Camera::SetHorizontalFoV(const float aHorizontalFoVRad, const Math::Vector2ui& aResolution)
	{
		myHorizontalFoVRad = aHorizontalFoVRad;

		if (myCameraType == eCameraType::Perspective)
		{
			SetPerspectiveProjection(aResolution);
		}
	}

	Math::Matrix4x4f Camera::GetMatrix() const
	{
		return myTransform.GetMatrix();
	}

	Math::Matrix4x4f Camera::GetProjectionMatrix() const
	{
		return myProjectionMatrix;
	}

	Math::Matrix4x4f Camera::GetViewMatrix() const
	{
		const Math::Vector3f& position = GetPosition();
		Math::Matrix4x4f viewMatrix;

		const Math::Vector3f forward = GetForward();
		const Math::Vector3f right = GetRight();
		const Math::Vector3f up = GetUp();

		viewMatrix(1, 1) = right.x;
		viewMatrix(2, 1) = right.y;
		viewMatrix(3, 1) = right.z;

		viewMatrix(1, 2) = up.x;
		viewMatrix(2, 2) = up.y;
		viewMatrix(3, 2) = up.z;

		viewMatrix(1, 3) = -forward.x;
		viewMatrix(2, 3) = -forward.y;
		viewMatrix(3, 3) = -forward.z;

		viewMatrix(4, 1) = Dot(-1.0f * position, right);
		viewMatrix(4, 1) = Dot(-1.0f * position, up);
		viewMatrix(4, 1) = Dot(position, forward);

		viewMatrix(1, 4) = 0.0f;
		viewMatrix(2, 4) = 0.0f;
		viewMatrix(3, 4) = 0.0f;
		viewMatrix(4, 4) = 1.0f;

		return viewMatrix;
	}

	float Camera::GetRotationSpeed() const
	{
		return myRotateSpeed;
	}

	Math::Vector3f Camera::GetPosition() const
	{
		return myTransform.GetPosition();
	}

	Math::Vector3f Camera::GetRotation() const
	{
		return myTransform.GetRotation();
	}

	float Camera::GetMoveSpeed() const
	{
		return myMoveSpeed;
	}

	float Camera::GetNearPlane() const
	{
		return myNearPlane;
	}

	float Camera::GetFarPlane() const
	{
		return myFarPlane;
	}

	float Camera::GetHorizontalFoV() const
	{
		return myHorizontalFoVRad;
	}

	bool Camera::IsFreeFlyActive() const
	{
		return myFreeFly;
	}

	Math::Vector3f Camera::GetForward() const
	{
		return myTransform.GetMatrix().GetForward();
	}

	Math::Vector3f Camera::GetUp() const
	{
		return myTransform.GetMatrix().GetUp();
	}

	Math::Vector3f Camera::GetRight() const
	{
		return myTransform.GetMatrix().GetRight();
	}
}