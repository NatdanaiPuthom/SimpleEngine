#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Engine/Graphics/Camera/Camera.hpp"

GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
	, myLevelManager(std::make_unique<LevelManager>())
	, myRaycastLine(std::make_unique<Drawer::Line>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();

	myRaycastLine->color = { 1,0,0, 1 };
	myRaycastLine->startPosition = { 1,0,-1 };
	myRaycastLine->endPosition = { 10,5,-1 };
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	myLevelManager->Init();
}

void GameWorld::Update()
{
	/*
	while (SimpleGlobal::GetGameIsRunning())
	{
	}
	*/

	myLevelManager->Update();

	if (SimpleUtilities::InputManager::GetInstance().IsPressed(VK_LBUTTON))
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

		bool hit = false;
		SU::Vector3f intersectPoint;

		for (const auto& test : SimpleWorld::GetActiveScene()->myModelInstances)
		{
			const auto min = test->GetBoundingBox().min;
			const auto max = test->GetBoundingBox().max;
			const auto worldMatrix = test->GetMatrix();

			SU::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
			SU::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

			minHomogeneous = worldMatrix * minHomogeneous;
			maxHomogeneous = worldMatrix * maxHomogeneous;

			SU::AABB3D aabb;
			aabb.InitWithMinAndMax(minHomogeneous.AsVector3(), maxHomogeneous.AsVector3());

			if (test->GetName() != "SkyBox")
			{
				if (SU::IntersectionAABBRay(aabb, ray, intersectPoint))
				{
					hit = true;
					std::cout << test->GetName() << std::endl;
				}
			}
		}

		if (hit)
		{
			SimpleGlobal::SetBoundingBoxLineColor({ 1,0,0 });
		}
		else
		{
			SimpleGlobal::SetBoundingBoxLineColor({ 0,1,0 });
		}

	}
}

void GameWorld::Render()
{
	myLevelManager->Render();
	SimpleGlobal::GetRenderer()->RenderLine(*myRaycastLine);
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}

void GameWorld::RenderReflection()
{
	//SimpleGlobal::GetRenderer()->RenderPlaneReflection(myPlaneReflection->myModelInstance.get());
}

void GameWorld::RenderUpSideDown()
{
	/*Renderer* renderer = SimpleGlobal::GetRenderer();
	for (const auto& model : myModelInstances)
	{
		renderer->RenderEverythingUpSideDown(model.get());
	}*/
}
