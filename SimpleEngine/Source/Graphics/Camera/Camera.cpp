#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	Camera::Camera(const float aFoV, const float aNearPlane, const float aFarPlane)
		: myFoV(aFoV)
		, myFarPlane(aFarPlane)
		, myNearPlane(aNearPlane)
		, myMoveSpeed(1.0f)
		, myRotateSpeed(45.0f)
		, myFreeFly(false)
		, myDebugCameraActive(false)
		, myInput(nullptr)
	{
		SetPosition({ 0,0,0 });
		CreateProjectionMatrix();
		UpdateCameraVectors();
	}

	void Camera::Init()
	{
		myInput = &SimpleUtilities::InputManager::GetInstance();
	}

	void Camera::Update(const float aDeltaTime)
	{
		const HWND& hwnd = Global::GetEngineHWND();

		if (GetForegroundWindow() != hwnd)
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

			if (myFreeFly && GetForegroundWindow() == hwnd)
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

	Math::Vector4f Camera::WorldToCameraSpace(const Math::Vector4f& aVector) const
	{
		return Math::Matrix4x4f::GetInverse(myTransform.GetMatrix()) * aVector;
	}

	Math::Vector4f Camera::CameraToProjectionSpace(const Math::Vector4f& aVector) const
	{
		Math::Vector4f projectedVector = myProjectionMatrix * aVector;
		projectedVector.x = projectedVector.x / projectedVector.z;
		projectedVector.y = projectedVector.y / projectedVector.z;

		return projectedVector;
	}

	Math::Vector2f Camera::ProjectionToPixel(const Math::Vector4f& aVector) const
	{
		const Math::Vector2ui resolutionUI(Global::GetResolution());
		const Math::Vector2f resolution(static_cast<float>(resolutionUI.x), static_cast<float>(resolutionUI.y));

		return Math::Vector2f(aVector.x * resolution.x / 2.0f + resolution.x / 2.0f, aVector.y * resolution.y / 2.0f + resolution.y / 2.0f);
	}

	Math::Matrix4x4f Camera::GetWorldToClipMatrix()
	{
		const Math::Matrix4x4f clipMatrix = Math::Matrix4x4f::GetInverse(myTransform.GetMatrix()) * myProjectionMatrix;
		return clipMatrix;
	}

	void Camera::CreateProjectionMatrix()
	{
		const Math::Vector2ui resolutionUI(Global::GetResolution());
		const Math::Vector2f resolution(static_cast<float>(resolutionUI.x), static_cast<float>(resolutionUI.y));

		myProjectionMatrix(1, 1) = 1 / tan(myFoV / 2);
		myProjectionMatrix(2, 2) = (resolution.x / resolution.y) * (1 / tan(myFoV / 2));
		myProjectionMatrix(3, 3) = myFarPlane / (myFarPlane - myNearPlane);
		myProjectionMatrix(3, 4) = 1;
		myProjectionMatrix(4, 3) = -myNearPlane * myFarPlane / (myFarPlane - myNearPlane);
		myProjectionMatrix(4, 4) = 0;
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

	void Camera::SetCameraValues(const Math::Vector3f& aPosition, const float aNearPlane, const float aFoV)
	{
		SetPosition(aPosition);

		myNearPlane = aNearPlane;
		myFoV = aFoV;

		CreateProjectionMatrix();
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

	void Camera::UpdateResolution()
	{
		CreateProjectionMatrix();
	}

	void Camera::InactiveFreeFly()
	{
		myInput->ShowMouse();
		myInput->ReleaseMouse();
		myFreeFly = false;
	}

	void Camera::SetNearPlane(const float aNearPlane)
	{
		myNearPlane = aNearPlane;

		CreateProjectionMatrix();
	}

	void Camera::SetMoveSpeed(const float aSpeed)
	{
		myMoveSpeed = aSpeed;
	}

	void Camera::SetRotateSpeed(const float aRotationSpeed)
	{
		myRotateSpeed = aRotationSpeed;
	}

	void Camera::SetFoV(const float aFoV)
	{
		myFoV = aFoV;

		CreateProjectionMatrix();
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
		return myFoV;
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