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
	, myLine(std::make_unique<Drawer::Line>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();

	myLine->color = { 1,0,0, 1 };
	myLine->startPosition = { 1,0,-1 };
	myLine->endPosition = { 10,5,-1 };
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
		myLine->endPosition = myLine->startPosition + mouseRay * 5000.0f;
	}
}

void GameWorld::Render()
{
	myLevelManager->Render();
	SimpleGlobal::GetRenderer()->RenderLine(*myLine);
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
