#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"

RaycastManager::RaycastManager()
	: myRaycastLine(std::make_unique<Drawer::Line>())
	, myDebugSphere(std::make_unique<Drawer::Sphere>())
	, mySelectedModelIndex(-1)
	, mySelectDelay(0.1f)
	, myTimer(0.0f)
{
	myRaycastLine->color = { 1,0,0, 1 };
	myRaycastLine->startPosition = { 0,0,0 };
	myRaycastLine->endPosition = { 0,0,0 };

	myDebugSphere->radius = 0.2f;
}

RaycastManager::~RaycastManager()
{
}

void RaycastManager::Update()
{
	if (SimpleUtilities::InputManager::GetInstance().IsHold(VK_LBUTTON))
	{
		if (mySelectedModelIndex >= 0 && myTimer >= mySelectDelay)
		{
			SimpleGlobal::SetBoundingBoxLineColor({ 1,0,0 });
			MoveObject();
		}
		else
		{
			CheckAABB3DCollision();
			myTimer += SimpleGlobal::GetDeltaTime();
		}
	}
	else
	{
		mySelectedModelIndex = -1;
		myTimer = 0;
		SimpleGlobal::SetBoundingBoxLineColor({ 1,1,0 });
	}
}

void RaycastManager::Render()
{
	SimpleGlobal::GetRenderer()->RenderLine(*myRaycastLine);
	SimpleGlobal::GetRenderer()->RenderSphere(*myDebugSphere);
}

SimpleUtilities::Ray RaycastManager::GetScreenPointToRay(const SimpleUtilities::Vector2f& aPosition)
{
	const auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

	const SU::Matrix4x4f projectionMatrix = camera->GetProjectionMatrix();
	const SU::Matrix4x4f viewMatrix = camera->GetViewMatrix();

	const SU::Vector2f resolution = SU::Vector2f(static_cast<float>(SimpleGlobal::GetResolution().x), static_cast<float>(SimpleGlobal::GetResolution().y));

	const float x = (2.0f * aPosition.x) / resolution.x - 1.0f;
	const float y = (1.0f - (2.0f * aPosition.y) / resolution.y) * -1.0f;

	SU::Vector4f clipCoords = { x, y, -1.0f, 1.0f };

	SU::Matrix4x4f invertedProjection = SU::Matrix4x4f::GetInverse(projectionMatrix);
	SU::Vector4f eyeCoords = invertedProjection * clipCoords;
	eyeCoords.z = -1.0f;
	eyeCoords.w = 0.0f;

	SU::Matrix4x4f invertedViewMatrix = SU::Matrix4x4f::GetInverse(viewMatrix);
	SU::Vector4f rayWorld = invertedViewMatrix * eyeCoords;

	SU::Vector3f mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay.Normalize();

	myRaycastLine->startPosition = camera->GetPosition();
	myRaycastLine->startPosition.x += 0.5f;
	myRaycastLine->endPosition = myRaycastLine->startPosition + mouseRay * 10000.0f;

	SU::Ray ray(camera->GetPosition(), mouseRay);
	return ray;
}

void RaycastManager::CheckAABB3DCollision()
{
	SU::Ray ray = GetScreenPointToRay(SU::InputManager::GetInstance().GetMousePosition());

	const auto& models = SimpleWorld::GetActiveScene()->myModelInstances;
	SU::Vector3f closetHitPoint;
	float closetDistance = FLT_MAX;
	int hitIndex = -1;

	for (unsigned int i = 0; i < models.size(); ++i)
	{
		const auto min = models[i]->GetBoundingBox().min;
		const auto max = models[i]->GetBoundingBox().max;
		const auto worldMatrix = models[i]->GetMatrix();

		SU::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
		SU::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

		minHomogeneous = worldMatrix * minHomogeneous;
		maxHomogeneous = worldMatrix * maxHomogeneous;

		SU::AABB3D aabb3d;
		aabb3d.InitWithMinAndMax(minHomogeneous.AsVector3(), maxHomogeneous.AsVector3());

		SU::Vector3f intersectionPoint;
		if (SU::IntersectionAABB3DRay(aabb3d, ray, intersectionPoint))
		{
			const float distance = SU::Distance(ray.GetOrigin(), intersectionPoint);

			if (distance < closetDistance)
			{
				closetDistance = distance;
				hitIndex = i;
			}
		}
	}

	if (hitIndex >= 0)
	{
		mySelectedModelIndex = hitIndex;
		myDebugSphere->position = models[mySelectedModelIndex]->GetPosition();
	}
	else
	{
		mySelectedModelIndex = -1;
		SimpleGlobal::SetBoundingBoxLineColor({ 1,1,0 });
	}
}

void RaycastManager::MoveObject()
{
	auto& model = SimpleWorld::GetActiveScene()->myModelInstances[mySelectedModelIndex];
	auto mouseDelta = SimpleUtilities::InputManager::GetInstance().GetMouseDelta();
	auto position = model->GetPosition();

	const auto min = model->GetBoundingBox().min;
	const auto max = model->GetBoundingBox().max;
	const auto worldMatrix = model->GetMatrix();

	SU::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
	SU::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

	minHomogeneous = worldMatrix * minHomogeneous;
	maxHomogeneous = worldMatrix * maxHomogeneous;

	SU::AABB3D aabb3d;
	aabb3d.InitWithMinAndMax(minHomogeneous.AsVector3(), maxHomogeneous.AsVector3());

	const SU::Ray ray = GetScreenPointToRay(SU::InputManager::GetInstance().GetMousePosition());

	SU::Vector3f intersectionPoint;
	if (SU::IntersectionAABB3DRay(aabb3d, ray, intersectionPoint))
	{

		position.x += intersectionPoint.x - position.x;
		position.z += intersectionPoint.z - position.z;
	}

	position.y += mouseDelta.y * 0.01f;

	model->SetPosition(position);
	myDebugSphere->position = position;
}