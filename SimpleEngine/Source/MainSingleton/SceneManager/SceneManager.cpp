#include "MainSingleton/SceneManager/SceneManager.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "External/profiler.h"

namespace Simpleton
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::Destroy()
	{
		myECSs.clear();
	}

	void SceneManager::Init()
	{
		auto camera = Global::GetGraphicsEngine()->GetEditorCamera();
		camera->SetRotation(Math::Vector3f(30, 0, 0));
		camera->SetPosition(Math::Vector3f(1, 9, -12));

		myECSs.emplace_back();
		myECSs[0].SetGlobalPointerToThis();
		myECSs[0].Init();

		PROFILER_BEGIN("ECS LoadData");
		ECS::EntityComponentSystem::LoadData(myECSs[0], "Assets/Scenes/Test_Scene.scene"); //NOTE(v11.0.6): move this once SceneManager is finish
		PROFILER_END();
	}

	void SceneManager::Update()
	{
		myECSs[0].Update();
	}

	void SceneManager::Render()
	{
		myECSs[0].Render();
	}

	void SceneManager::LateRender()
	{
		myECSs[0].RenderPointLights();
		myECSs[0].RenderSkyBoxAndDirectionalLight();
	}
}