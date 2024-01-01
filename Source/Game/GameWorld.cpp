#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/Special/PlaneReflection.h"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myImGuiManager(std::make_unique<ImGuiManager>())
		, myLevelManager(std::make_unique<LevelManager>())
		, myRaycastManager(std::make_unique<RaycastManager>())
	{
		SimpleWorldGameWorldImpl::SetGameWorld(this);

		myPlaneReflection = std::make_unique<PlaneReflection>();
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
		myRaycastManager->Update();
	}

	void GameWorld::Render()
	{
		myLevelManager->Render();
		myRaycastManager->Render();
	}

	void GameWorld::RenderImGui()
	{
		myImGuiManager->Render();
	}

	void GameWorld::RenderReflection()
	{
		SimpleGlobal::GetRenderer()->RenderPlaneReflection(myPlaneReflection->myModel.get());
	}

	void GameWorld::RenderUpSideDown()
	{
		auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

		const SU::Vector3f oldCamPosition = camera->GetPosition();
		const SU::Vector3f oldCamRotation = camera->GetRotation();
		const SU::Vector3f newCamRotation = SimpleUtilities::Vector3f(oldCamRotation.x, -oldCamRotation.y, oldCamRotation.z);

		const float waterHeight = myPlaneReflection->myModel->GetPosition().y;
		const float distFromWater = 2.0f * (oldCamPosition.y - waterHeight);

		camera->SetPosition(oldCamPosition - SimpleUtilities::Vector3f(0.0f, distFromWater, 0.0f));
		camera->SetRotation(newCamRotation);

		Simple::Renderer* renderer = SimpleGlobal::GetRenderer();
		for (const auto& model : SimpleWorld::GetActiveScene()->myModels)
		{
			renderer->RenderEverythingUpSideDown(model.get());
		}

		camera->SetRotation(oldCamRotation);
		camera->SetPosition(oldCamPosition);
	}
}