#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Model/Mesh.hpp"


GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
	, myLevelManager(std::make_unique<LevelManager>())
	, myLine(std::make_unique<Drawer::Line>())
	, myLine2(std::make_unique<Drawer::Line>())
	, myLine3(std::make_unique<Drawer::Line>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();

	myLine->color = { 1,0,0, 1 };
	myLine2->color = { 0,1,1, 1 };
	myLine->startPosition = { 1,0,-1 };
	myLine->endPosition = { 10,5,-1 };

	myLine3->color = { 1,1,1, 1 };
	myLine3->startPosition = { 0,0,0 };
	myLine3->endPosition = { 0,10,0 };
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


	auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

	SimpleUtilities::Matrix4x4f projectionMatrix = camera->GetProjectionMatrix();
	SimpleUtilities::Matrix4x4f viewMatrix = camera->GetViewMatrix();

	SimpleUtilities::Vector2f resolution = SimpleUtilities::Vector2f(static_cast<float>(SimpleGlobal::GetResolution().x), static_cast<float>(SimpleGlobal::GetResolution().y));
	SimpleUtilities::Vector2f mousePos = SimpleUtilities::InputManager::GetInstance().GetMousePosition();

	float x = (2.0f * mousePos.x) / resolution.x - 1.0f;
	float y = (1.0f - (2.0f * mousePos.y) / resolution.y) * -1.0f;

	SimpleUtilities::Vector4f clipCoords = { x, y, -1.0f, 1.0f };

	SimpleUtilities::Matrix4x4f invertedProjection = SimpleUtilities::Matrix4x4f::GetInverse(projectionMatrix);
	SimpleUtilities::Vector4f eyeCoords = invertedProjection * clipCoords;
	eyeCoords.z = -1.0f;
	eyeCoords.w = 0.0f;

	SimpleUtilities::Matrix4x4f invertedViewMatrix = SimpleUtilities::Matrix4x4f::GetInverse(viewMatrix);
	SimpleUtilities::Vector4f rayWorld = invertedViewMatrix * eyeCoords;

	SimpleUtilities::Vector3f mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay.Normalize();

	myRay.InitWithOriginAndDirection(camera->GetPosition(), mouseRay);

	myLine->startPosition = camera->GetPosition();
	myLine->endPosition = myLine->startPosition + mouseRay * 10000.0f;

	//std::cout << "Mouse Dir: " << mouseRay << std::endl;
	//std::cout << "start:" << myLine->startPosition << std::endl;
	//std::cout << "end:" << myLine->endPosition << std::endl;

	const auto& pyramid = SimpleWorld::GetActiveScene()->myModelInstances[0];

	SimpleUtilities::Vector4f pos(pyramid->GetPosition().x, pyramid->GetPosition().y, pyramid->GetPosition().z, 1.0f);

	pos = pyramid->GetMatrix().GetInverse(pyramid->GetMatrix()) * pos;


	myLine2->startPosition = pos.AsVector3();
	pos.y += 100;
	myLine2->endPosition = pos.AsVector3();


	bool hit = false;
	SimpleUtilities::Vector3f intersectPoint;
	for (const auto& test : SimpleWorld::GetActiveScene()->myModelInstances)
	{
		const auto min = test->GetMesh()->GetBoundingBox().min;
		const auto max = test->GetMesh()->GetBoundingBox().max;
		const auto worldMatrix = test->GetMatrix();

		SimpleUtilities::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
		SimpleUtilities::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

		minHomogeneous = worldMatrix * minHomogeneous;
		maxHomogeneous = worldMatrix * maxHomogeneous;

		/*min.x = minHomogeneous.x / minHomogeneous.w;
		min.y = minHomogeneous.y / minHomogeneous.w;
		min.z = minHomogeneous.z / minHomogeneous.w;

		max.x = maxHomogeneous.x / maxHomogeneous.w;
		max.y = maxHomogeneous.y / maxHomogeneous.w;
		max.z = maxHomogeneous.z / maxHomogeneous.w;*/

		SimpleUtilities::AABB3D aabb;
		aabb.InitWithMinAndMax(minHomogeneous.AsVector3(), maxHomogeneous.AsVector3());

		
		if (test->GetName() != "SkyBox")
		{
			if (SimpleUtilities::IntersectionAABBRay(aabb, myRay, intersectPoint))
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

void GameWorld::Render()
{
	myLevelManager->Render();
	SimpleGlobal::GetRenderer()->RenderLine(*myLine);
	SimpleGlobal::GetRenderer()->RenderLine(*myLine2);
	SimpleGlobal::GetRenderer()->RenderLine(*myLine3);
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
