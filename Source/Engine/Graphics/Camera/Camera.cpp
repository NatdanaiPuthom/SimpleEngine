#include "Engine/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

Camera::Camera(const SimpleUtilities::Vector2f& aResolution, const float aFoV, const float aNearPlane, const float aFarPlane)
	: myResolution(aResolution)
	, myFoV(aFoV)
	, myFarPlane(aFarPlane)
	, myNearPlane(aNearPlane)
	, myMoveSpeed(10)
	, myRotateSpeed(90)
	, myInput(&SimpleUtilities::InputManager::GetInstance())
	, myFreeFly(false)
{
	SetPosition({ 0,0,0 });
	CreateProjectionMatrix();
	UpdateCameraVectors();
}

void Camera::Update(const float aDeltaTime)
{
	if (myInput->GetAKeyIsPressed() || myFreeFly)
	{
		const SimpleUtilities::Vector3f currentPosition = GetPosition();
		const SimpleUtilities::Vector3f currentRotation = GetRotation();

		SimpleUtilities::Vector3f forward;
		SimpleUtilities::Vector3f targetPosition(currentPosition);
		SimpleUtilities::Vector3f targetRotation(currentRotation);

		float speed = myMoveSpeed;

		if (myInput->IsHold(VK_SHIFT))
		{
			speed *= 3.0f;
		}

		if (myInput->IsPressed(VK_TAB))
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

		if (myInput->IsHold('W'))
		{
			forward = myForward * speed * aDeltaTime;
			targetPosition += forward;
		}

		if (myInput->IsHold('S'))
		{
			forward = -1.0f * myForward * speed * aDeltaTime;
			targetPosition += forward;
		}

		if (myInput->IsHold('A'))
		{
			forward = -1.0f * myRight * speed * aDeltaTime;
			targetPosition += forward;
		}

		if (myInput->IsHold('D'))
		{
			forward = myRight * speed * aDeltaTime;
			targetPosition += forward;
		}

		if (myInput->IsHold('Q'))
		{
			targetRotation.y -= myRotateSpeed * aDeltaTime;
		}

		if (myInput->IsHold('E'))
		{
			targetRotation.y += myRotateSpeed * aDeltaTime;
		}

		if (myInput->IsHold('Z'))
		{
			targetRotation.x -= myRotateSpeed * aDeltaTime;
		}

		if (myInput->IsHold('C'))
		{
			targetRotation.x += myRotateSpeed * aDeltaTime;
		}

		float direction = 1.0f;

		if (myInput->IsHold(VK_CONTROL))
		{
			direction = -1.0f;
		}

		if (myInput->IsHold(VK_SPACE))
		{
			targetPosition.y += direction * speed * myUp.y * aDeltaTime;
		}

		HWND& hwnd = SimpleGlobal::GetHWND();
		if (myFreeFly && GetForegroundWindow() == hwnd)
		{
			if (!myInput->GetMouseIsHidden())
			{
				SimpleUtilities::Vector2f pos = myInput->GetMousePosition();
				myCapturedPosition.x = static_cast<int>(pos.x);
				myCapturedPosition.y = static_cast<int>(pos.y);

				myInput->HideMouse();
				myInput->CaptureMouse();
			}

			SetCursorPos(myCapturedPosition.x, myCapturedPosition.y);

			SimpleUtilities::Vector2f mouseDelta = myInput->GetMouseDelta();
			mouseDelta *= myRotateSpeed * 0.0015f;

			targetRotation.x += mouseDelta.y;
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

		targetRotation = SimpleUtilities::SmoothStep(currentRotation, targetRotation, 0.3f);
		targetPosition = SimpleUtilities::SmoothStep(currentPosition, targetPosition, 0.3f);

		SetRotation(targetRotation);
		SetPosition(targetPosition);
	}
}

SimpleUtilities::Vector4f Camera::WorldToCameraSpace(const SimpleUtilities::Vector4f& aVector) const
{
	return SimpleUtilities::Matrix4x4f::GetFastInverse(myTransform.GetMatrix()) * aVector;
}

SimpleUtilities::Vector4f Camera::CameraToProjectionSpace(const SimpleUtilities::Vector4f& aVector) const
{
	SimpleUtilities::Vector4f projectedVector = myProjectionMatrix * aVector;
	projectedVector.x = projectedVector.x / projectedVector.z;
	projectedVector.y = projectedVector.y / projectedVector.z;

	return projectedVector;
}

SimpleUtilities::Vector2f Camera::ProjectionToPixel(const SimpleUtilities::Vector4f& aVector) const
{
	return SimpleUtilities::Vector2f(aVector.x * myResolution.x / 2.0f + myResolution.x / 2.0f, aVector.y * myResolution.y / 2.0f + myResolution.y / 2.0f);
}

SimpleUtilities::Matrix4x4f Camera::GetWorldToClipMatrix()
{
	const SimpleUtilities::Matrix4x4f clipMatrix = myTransform.GetMatrix().GetFastInverse() * myProjectionMatrix;
	return clipMatrix;
}

void Camera::CreateProjectionMatrix()
{
	myProjectionMatrix(1, 1) = 1 / tan(myFoV / 2);
	myProjectionMatrix(2, 2) = (myResolution.x / myResolution.y) * (1 / tan(myFoV / 2));
	myProjectionMatrix(3, 3) = myFarPlane / (myFarPlane - myNearPlane);
	myProjectionMatrix(3, 4) = 1;
	myProjectionMatrix(4, 3) = -myNearPlane * myFarPlane / (myFarPlane - myNearPlane);
	myProjectionMatrix(4, 4) = 0;
}

void Camera::UpdateCameraVectors()
{
	const SimpleUtilities::Matrix4x4f modelToWorldMatrix = myTransform.GetMatrix();

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

void Camera::SetCameraValues(const SimpleUtilities::Vector3f& aPosition, const SimpleUtilities::Vector2f& aResolution, const float aNearPlane, const float aFoV)
{
	SetPosition(aPosition);

	myResolution = aResolution;
	myNearPlane = aNearPlane;
	myFoV = aFoV;

	CreateProjectionMatrix();
}

void Camera::SetPosition(const SimpleUtilities::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
	UpdateCameraVectors();
}

void Camera::SetRotation(const SimpleUtilities::Vector3f aRotationInDegree)
{
	myTransform.SetRotation(aRotationInDegree);
}

void Camera::SetResolution(const SimpleUtilities::Vector2f& aResolution)
{
	myResolution = aResolution;

	CreateProjectionMatrix();
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

SimpleUtilities::Matrix4x4f Camera::GetModelToWorldMatrix() const
{
	return myTransform.GetMatrix();
}

SimpleUtilities::Matrix4x4f Camera::GetProjectionMatrix() const
{
	return myProjectionMatrix;
}

SimpleUtilities::Matrix4x4f Camera::GetViewMatrix() const
{
	const SimpleUtilities::Vector3f& position = GetPosition();
	SimpleUtilities::Matrix4x4f viewMatrix;

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

SimpleUtilities::Vector3f Camera::GetPosition() const
{
	return myTransform.GetPosition();
}

SimpleUtilities::Vector3f Camera::GetRotation() const
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

SimpleUtilities::Vector3f Camera::GetForward() const
{
	return myForward;
}

SimpleUtilities::Vector3f Camera::GetUp() const
{
	return myUp;
}

SimpleUtilities::Vector3f Camera::GetRight() const
{
	return myRight;
}

SimpleUtilities::Vector2f Camera::GetResolution() const
{
	return myResolution;
}
