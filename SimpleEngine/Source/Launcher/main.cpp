#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Test/ECS/ECS.hpp"
#include "Editor/Editor.hpp"

#include "Engine/Components/MeshComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

static void Run(HINSTANCE& hInstance, int nCmdShow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	SimpleTracker::MemoryTrackingSettings memoryTrackerSettings = {};
	memoryTrackerSettings.myShouldStoreStackTraces = false;
	memoryTrackerSettings.myShouldTrackAllAllocations = true;
	SimpleTracker::StartMemoryTracking(memoryTrackerSettings);

	PROFILER_INIT();
	PROFILER_ENABLE();
	PROFILER_START_LISTEN();
	PROFILER_BEGIN("Main.cpp");

	Run(hInstance, nCmdShow);

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}

static void Run(HINSTANCE& hInstance, int nCmdShow)
{
	PROFILER_BEGIN("Engine initialize");
	Simple::Engine engine;
	Simple::GraphicsEngine graphicsEngine;
	Simple::ECS ecs;
	Simple::Editor editor;

	engine.SetGlobalPointerToThis();
	graphicsEngine.SetGlobalGraphicsEngineToThis();
	ecs.SetGlobalECSPointerToThis();

	engine.Init(hInstance, nCmdShow);
	graphicsEngine.Init(Global::GetWindowSize(), Global::GetEngineHWND());
	ecs.Init();
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	Simple::Entity entity = ecs.CreateEntity();
	entity->AddComponent<MeshComponent>();
	entity->AddComponent<TransformComponent>();

	/*for (size_t i = 0; i < 1; ++i)
	{
		Simple::EntityClass entity2 = ecs.CreateEntity();
		entity2;
	}*/

	MeshComponent* meshComponent = entity->GetComponent<MeshComponent>();
	TransformComponent* transformComponent = entity->GetComponent<TransformComponent>();

	std::string path = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_MODELS) + "StaticModels/Simple_Floor_10x10.fbx";

	meshComponent->mesh = new Simple::Mesh(Global::GetModelFactory()->LoadMeshTest(path));
	meshComponent->shader = Global::GetGraphicsEngine()->GetDefaultShader().get();
	meshComponent->texture = Global::GetGraphicsEngine()->GetTexture("DefaultTexture.dds").get();

	while (Global::GetGameIsRunning())
	{
		PROFILER_FUNCTION(profiler::colors::Blue);

		if (graphicsEngine.BeginFrame() == false)
			continue;

		engine.Update();
		gameWorld.Update();
		ecs.Update();
		editor.Update();

		gameWorld.Render();
		ecs.Render();
		editor.Render();

		Global::GetRenderer()->TestRender(transformComponent, meshComponent);
	
		graphicsEngine.EndFrame();
	}

	delete meshComponent->mesh;
}