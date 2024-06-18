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

		myCurrentScene = "Assets/Scenes/Test_Scene.scene";
		myECSs.try_emplace(myCurrentScene);

		myECSs[myCurrentScene].SetGlobalPointerToThis();
		myECSs[myCurrentScene].Init();

		PROFILER_BEGIN("ECS LoadData");
		ECS::EntityComponentSystem::LoadData(myECSs[myCurrentScene], myCurrentScene);
		PROFILER_END();
	}

	void SceneManager::Update()
	{
		myECSs[myCurrentScene].Update();
	}

	void SceneManager::Render()
	{
		myECSs[myCurrentScene].Render();
	}

	void SceneManager::LateRender()
	{
		myECSs[myCurrentScene].RenderPointLights();
		myECSs[myCurrentScene].RenderSkyBoxAndDirectionalLight();
	}
}