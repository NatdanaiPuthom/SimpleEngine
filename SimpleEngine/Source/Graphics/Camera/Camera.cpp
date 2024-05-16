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
		, myDebugCameraActive(false)
		, myInput(nullptr)
	{
		SetPosition({ 0,0,0 });
		SetPerspectiveProjection({ 1280, 720 });
		UpdateCameraVectors();
	}

	void Camera::Init()
	{
		myInput = &MainSingleton::GetInputManager();
	}

	void Camera::Update(const float aDeltaTime, const HWND& aHWND)
	{
		if (GetForegroundWindow() != aHWND)
		{
			if (myInput->GetMouseIsHidden())
			{
				myInput->ShowMouse();
				myInput->ReleaseMouse();
			}

			myFreeFly = false;
			myDebugCameraActive = false;
			return;
		}

		if (myInput->IsKeyPressed(VK_TAB) == true)
		{
			myDebugCameraActive = !myDebugCameraActive;
		}

		if (myDebugCameraActive == true && myInput->GetAKeyIsPressed() || myFreeFly)
		{
			const Math::Vector3f currentPosition = GetPosition();
			const Math::Vector3f currentRotation = GetRotation();

			Math::Vector3f forward;
			Math::Vector3f targetPosition(currentPosition);
			Math::Vector3f targetRotation(currentRotation);

			float speed = myMoveSpeed;

			if (myInput->IsKeyHeld(VK_SHIFT))
			{
				speed *= 5.0f;
			}

			if (myInput->IsKeyPressed(VK_TAB))
			{
				if (!myFreeFly)
				{
					myFreeFly = true;
				}
				else
				{
					myFreeFly = false;
				}
			}

			const int mouseWheelDelta = myInput->GetMouseWheelDelta();
			myMoveSpeed += mouseWheelDelta;

			if (myMoveSpeed < 0)
			{
				myMoveSpeed = 0;
			}

			if (myInput->IsKeyHeld('W'))
			{
				forward = myForward * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('S'))
			{
				forward = -1.0f * myForward * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('A'))
			{
				forward = -1.0f * myRight * speed * aDeltaTime;
				targetPosition += forward;
			}

			if (myInput->IsKeyHeld('D'))
			{
				forward = myRight * speed * aDeltaTime;
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
				targetPosition.y += direction * speed * myUp.y * aDeltaTime;
			}

			if (myFreeFly)
			{
				if (!myInput->GetMouseIsHidden())
				{
					Math::Vector2f pos = myInput->GetMousePosition();
					myCapturedPosition.x = static_cast<int>(pos.x);
					myCapturedPosition.y = static_cast<int>(pos.y);

					myInput->HideMouse();
					myInput->CaptureMouse();
				}

				SetCursorPos(myCapturedPosition.x, myCapturedPosition.y);

				Math::Vector2f mouseDelta = myInput->GetMouseDelta();
				mouseDelta *= myRotateSpeed * 0.0015f;

				targetRotation.x += -mouseDelta.y; //I don't know why flip
				targetRotation.y += mouseDelta.x;
			}
			else
			{
				if (myInput->GetMouseIsHidden())
				{
					myInput->ShowMouse();
					myInput->ReleaseMouse();
				}
			}

			SetRotation(targetRotation);
			SetPosition(targetPosition);
		}
	}

	Math::Matrix4x4f Camera::GetWorldToClipMatrix()
	{
		const Math::Matrix4x4f clipMatrix = Math::Matrix4x4f::GetInverse(myTransform.GetMatrix()) * myProjectionMatrix;
		return clipMatrix;
	}

	void Camera::SetPerspectiveProjection(const Math::Vector2ui aResolution)
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

	void Camera::UpdateCameraVectors()
	{
		const Math::Matrix4x4f modelToWorldMatrix = myTransform.GetMatrix();

		myForward.x = modelToWorldMatrix(3, 1);
		myForward.y = modelToWorldMatrix(3, 2);
		myForward.z = modelToWorldMatrix(3, 3);
		myForward.Normalize();

		myRight.x = modelToWorldMatrix(1, 1);
		myRight.y = modelToWorldMatrix(1, 2);
		myRight.z = modelToWorldMatrix(1, 3);
		myRight.Normalize();

		myUp.x = modelToWorldMatrix(2, 1);
		myUp.y = modelToWorldMatrix(2, 2);
		myUp.z = modelToWorldMatrix(2, 3);
		myUp.Normalize();
	}

	void Camera::SetPosition(const Math::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
		UpdateCameraVectors();
	}

	void Camera::SetRotation(const Math::Vector3f aRotationInDegree)
	{
		myTransform.SetRotation(aRotationInDegree);
	}

	void Camera::UpdateResolution(const Math::Vector2ui aResolution)
	{
		SetPerspectiveProjection(aResolution);
	}

	void Camera::InactiveFreeFly()
	{
		myInput->ShowMouse();
		myInput->ReleaseMouse();
		myFreeFly = false;
	}

	void Camera::SetNearPlane(const float aNearPlane, const Math::Vector2ui aResolution)
	{
		myNearPlane = aNearPlane;
		SetPerspectiveProjection(aResolution);
	}

	void Camera::SetMoveSpeed(const float aSpeed)
	{
		myMoveSpeed = aSpeed;
	}

	void Camera::SetRotateSpeed(const float aRotationSpeed)
	{
		myRotateSpeed = aRotationSpeed;
	}

	void Camera::SetHorizontalFoV(const float aHorizontalFoVRad, const Math::Vector2ui aResolution)
	{
		myHorizontalFoVRad = aHorizontalFoVRad;
		SetPerspectiveProjection(aResolution);
	}

	Math::Matrix4x4f Camera::GetModelToWorldMatrix() const
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

		viewMatrix(1, 1) = myRight.x;
		viewMatrix(2, 1) = myRight.y;
		viewMatrix(3, 1) = myRight.z;

		viewMatrix(1, 2) = myUp.x;
		viewMatrix(2, 2) = myUp.y;
		viewMatrix(3, 2) = myUp.z;

		viewMatrix(1, 3) = -myForward.x;
		viewMatrix(2, 3) = -myForward.y;
		viewMatrix(3, 3) = -myForward.z;

		viewMatrix(4, 1) = Dot(-1.0f * position, myRight);
		viewMatrix(4, 1) = Dot(-1.0f * position, myUp);
		viewMatrix(4, 1) = Dot(position, myForward);

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

	float Camera::GetFoV() const
	{
		return myHorizontalFoVRad;
	}

	bool Camera::IsFreeFlyActive() const
	{
		return myFreeFly;
	}

	Math::Vector3f Camera::GetForward() const
	{
		return myForward;
	}

	Math::Vector3f Camera::GetUp() const
	{
		return myUp;
	}

	Math::Vector3f Camera::GetRight() const
	{
		return myRight;
	}
}