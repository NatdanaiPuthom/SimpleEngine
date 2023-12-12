#include "stdafx.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/SimpleUtilities/Utility.h"

Camera::Camera(const SimpleUtilities::Vector2f& aResolution, const float aFoV, const float aNearPlane, const float aFarPlane)
	: myResolution(aResolution)
	, myFoV(aFoV)
	, myFarPlane(aFarPlane)
	, myNearPlane(aNearPlane)
	, myMoveSpeed(3)
	, myRotateSpeed(30)
	, myInput(&SimpleUtilities::InputManager::GetInstance())
	, myFreeFly(false)
{
	SetPosition({ 0,0,0 });
	CreateProjectionMatrix();
	UpdateCameraVectors();
}

void Camera::Update(const float aDeltaTime)
{
	if (myInput->GetAKeyIsPressed())
	{
		SimpleUtilities::Vector3f forward;
		SimpleUtilities::Vector3f targetPosition(myModelToWorldTransform(4, 1), myModelToWorldTransform(4, 2), myModelToWorldTransform(4, 3));

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

		//TO-DO: Use New SetRotation function from Matrix4x4
		if (myInput->IsHold('Q'))
		{
			SetPosition(SimpleUtilities::Vector3f(0.0f, 0.0f, 0.0f));
			myModelToWorldTransform *= SimpleUtilities::Matrix4x4f::CreateRotationAroundY(-myRotateSpeed * 3.14f / 180.0f * aDeltaTime);
			SetPosition(targetPosition);
		}

		if (myInput->IsHold('E'))
		{
			SetPosition(SimpleUtilities::Vector3f(0.0f, 0.0f, 0.0f));
			myModelToWorldTransform *= SimpleUtilities::Matrix4x4f::CreateRotationAroundY(myRotateSpeed * 3.14f / 180.0f * aDeltaTime);
			SetPosition(targetPosition);
		}

		if (myInput->IsHold('Z'))
		{
			SetPosition(SimpleUtilities::Vector3f(0.0f, 0.0f, 0.0f));
			myModelToWorldTransform *= SimpleUtilities::Matrix4x4f::CreateRotationAroundX(myRotateSpeed * 3.14f / 180.0f * aDeltaTime);
			SetPosition(targetPosition);
		}

		if (myInput->IsHold('C'))
		{
			SetPosition(SimpleUtilities::Vector3f(0.0f, 0.0f, 0.0f));
			myModelToWorldTransform *= SimpleUtilities::Matrix4x4f::CreateRotationAroundX(-myRotateSpeed * 3.14f / 180.0f * aDeltaTime);
			SetPosition(targetPosition);
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

		HWND& hwnd = SimplyGlobal::GetHWND();
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

			//TODO: Add Freefly with Mouse
			SimpleUtilities::Vector2f mouseDelta = myInput->GetMouseDelta();
			mouseDelta *= myRotateSpeed * 3.14f / 180.0f * aDeltaTime;

			SetPosition(SimpleUtilities::Vector3f(0.0f, 0.0f, 0.0f));
			myModelToWorldTransform *= SimpleUtilities::Matrix4x4f::CreateRotationAroundY(mouseDelta.y * 0.5f);
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

		SetPosition(targetPosition);
	}
}

SimpleUtilities::Vector4f Camera::WorldToCameraSpace(const SimpleUtilities::Vector4f& aVector) const
{
	return SimpleUtilities::Matrix4x4f::GetFastInverse(myModelToWorldTransform) * aVector;
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
	const SimpleUtilities::Matrix4x4f clipMatrix = myModelToWorldTransform.GetFastInverse() * myProjectionMatrix;
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
	myForward.x = myModelToWorldTransform(3, 1);
	myForward.y = myModelToWorldTransform(3, 2);
	myForward.z = myModelToWorldTransform(3, 3);
	myForward.Normalize();

	myRight.x = myModelToWorldTransform(1, 1);
	myRight.y = myModelToWorldTransform(1, 2);
	myRight.z = myModelToWorldTransform(1, 3);
	myRight.Normalize();

	myUp.x = myModelToWorldTransform(2, 1);
	myUp.y = myModelToWorldTransform(2, 2);
	myUp.z = myModelToWorldTransform(2, 3);
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
	myModelToWorldTransform.SetPosition(aPosition);
	UpdateCameraVectors();
}

void Camera::SetRotation(const SimpleUtilities::Vector3f aRotation)
{
	myModelToWorldTransform.SetRotation(aRotation);
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

void Camera::SetFoV(const float aFoV)
{
	myFoV = aFoV;

	CreateProjectionMatrix();
}

SimpleUtilities::Matrix4x4f Camera::GetModelToWorldMatrix() const
{
	return myModelToWorldTransform;
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

	viewMatrix(4, 1) = -position.Dot(myRight);
	viewMatrix(4, 1) = -position.Dot(myUp);
	viewMatrix(4, 1) = position.Dot(myForward);

	viewMatrix(1, 4) = 0.0f;
	viewMatrix(2, 4) = 0.0f;
	viewMatrix(3, 4) = 0.0f;
	viewMatrix(4, 4) = 1.0f;

	return viewMatrix;
}

SimpleUtilities::Vector3f Camera::GetPosition() const
{
	return myModelToWorldTransform.GetPosition();;
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
