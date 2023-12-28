#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"

RaycastManager::RaycastManager()
	: myRaycastLine(std::make_unique<Drawer::Line>())
{
	myRaycastLine->color = { 1,0,0, 1 };
	myRaycastLine->startPosition = { 0,0,0 };
	myRaycastLine->endPosition = { 0,0,0 };
}

RaycastManager::~RaycastManager()
{
}

void RaycastManager::Update()
{
	if (SimpleUtilities::InputManager::GetInstance().IsHold(VK_LBUTTON))
	{
		const auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

		const SU::Matrix4x4f projectionMatrix = camera->GetProjectionMatrix();
		const SU::Matrix4x4f viewMatrix = camera->GetViewMatrix();

		const SU::Vector2f resolution = SU::Vector2f(static_cast<float>(SimpleGlobal::GetResolution().x), static_cast<float>(SimpleGlobal::GetResolution().y));
		const SU::Vector2f mousePos = SU::InputManager::GetInstance().GetMousePosition();

		const float x = (2.0f * mousePos.x) / resolution.x - 1.0f;
		const float y = (1.0f - (2.0f * mousePos.y) / resolution.y) * -1.0f;

		SU::Vector4f clipCoords = { x, y, -1.0f, 1.0f };

		SU::Matrix4x4f invertedProjection = SU::Matrix4x4f::GetInverse(projectionMatrix);
		SU::Vector4f eyeCoords = invertedProjection * clipCoords;
		eyeCoords.z = -1.0f;
		eyeCoords.w = 0.0f;

		SU::Matrix4x4f invertedViewMatrix = SU::Matrix4x4f::GetInverse(viewMatrix);
		SU::Vector4f rayWorld = invertedViewMatrix * eyeCoords;

		SU::Vector3f mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
		mouseRay.Normalize();

		SU::Ray ray(camera->GetPosition(), mouseRay);

		myRaycastLine->startPosition = camera->GetPosition();
		myRaycastLine->startPosition.x += 0.5f;
		myRaycastLine->endPosition = myRaycastLine->startPosition + mouseRay * 10000.0f;

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
			SimpleGlobal::SetBoundingBoxLineColor({ 1,0,0 });
			//std::cout << models[hitIndex]->GetName() << std::endl;
		}
		else
		{
			SimpleGlobal::SetBoundingBoxLineColor({ 1,1,0 });
		}
	}
}

void RaycastManager::Render()
{
	SimpleGlobal::GetRenderer()->RenderLine(*myRaycastLine);
}
