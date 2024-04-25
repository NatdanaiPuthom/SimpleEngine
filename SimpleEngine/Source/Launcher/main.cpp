#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Editor/Editor.hpp"

#include "Game/Test/ECS2/ComponentM.hpp"
#include "Game/Test/ECS2/EntityM.hpp"

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
	Simple::Editor editor;

	engine.SetGlobalPointerToThis();
	graphicsEngine.SetGlobalGraphicsEngineToThis();

	engine.Init(hInstance, nCmdShow);
	graphicsEngine.Init(Global::GetWindowSize(), Global::GetEngineHWND());
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	struct HelloW
	{
		int a;
	};

	HelloW he;
	he.a = 9;

	Simple::ComponentM compM;

	Simple::ComponentID id1 = compM.CreateComponent<HelloW>(he);
	HelloW* hello1 = compM.GetComponentByComponentID<HelloW>(id1);
	hello1->a = 10;

	Simple::ComponentID id2 = compM.CreateComponent<HelloW>(he);
	HelloW* hello2 = compM.GetComponentByComponentID<HelloW>(id2);
	hello2->a = 20
		;
	Simple::ComponentID id3 = compM.CreateComponent<HelloW>(he);
	HelloW* hello3 = compM.GetComponentByComponentID<HelloW>(id3);
	hello3->a = 30;

	id1; id2; id3;

	Simple::EntityM entityM;

	Simple::EntityE& entity = entityM.CreateEntity();
	entity;



	while (Global::GetGameIsRunning())
	{
		PROFILER_FUNCTION(profiler::colors::Blue);

		if (graphicsEngine.BeginFrame() == false)
			continue;

		engine.Update();
		gameWorld.Update();
		editor.Update();

		gameWorld.Render();
		editor.Render();
	
		graphicsEngine.EndFrame();
	}
}