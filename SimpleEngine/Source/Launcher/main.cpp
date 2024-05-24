#include "MainSingleton/MainSingleton.hpp"
#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Editor/Editor.hpp"

//#include "NodeScript/SimpleNodeScript.hpp"

static void Run(HINSTANCE& hInstance, int nCmdShow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	SimpleTracker::MemoryTrackingSettings memoryTrackerSettings = {};
	memoryTrackerSettings.myShouldStoreStackTraces = false;
	memoryTrackerSettings.myShouldTrackAllAllocations = true;
	SimpleTracker::StartMemoryTracking(memoryTrackerSettings);
	std::atexit(SimpleTracker::StopMemoryTrackingAndPrint);

	PROFILER_INIT();
	PROFILER_ENABLE();
	PROFILER_START_LISTEN();
	PROFILER_BEGIN("Main.cpp");

	Run(hInstance, nCmdShow);

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	MainSingleton::Release();

	return 0;
}

static void Run(HINSTANCE& hInstance, int nCmdShow)
{
	PROFILER_BEGIN("Engine initialize");
	MainSingleton::Init();

	Simple::Engine engine;
	Graphics::GraphicsEngine graphicsEngine;
	Editor::EditorEngine editor;
	ECS::EntityComponentSystem ecs;

	engine.SetGlobalPointerToThis();
	graphicsEngine.SetGlobalGraphicsEngineToThis();
	ecs.SetGlobalPointerToThis();

	engine.Init(hInstance, nCmdShow);
	graphicsEngine.Init(Global::GetEngineHWND(), Global::GetWindowSize());
	ecs.Init();
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	//Script::SimpleNodeScript simpleScript;
	//simpleScript.Init();

	while (Global::GetGameIsRunning())
	{
		PROFILER_FUNCTION(profiler::colors::Blue);

		PROFILER_BEGIN("BeginFrame");
		if (graphicsEngine.BeginFrame() == false)
		{
			PROFILER_END();
			continue;
		}
		PROFILER_END();

		PROFILER_BEGIN("Engine Update");
		engine.Update();
		PROFILER_END();

		PROFILER_BEGIN("Game Update");
		ecs.Update();
		gameWorld.Update();
		PROFILER_END();

		PROFILER_BEGIN("Editor Update");
		//simpleScript.Update();
		editor.Update();
		PROFILER_END();

		PROFILER_BEGIN("Render To Backbuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Backbuffer, graphicsEngine.GetDepthBuffer().Get());
		ecs.Render();
		gameWorld.Render();
		PROFILER_END();

		PROFILER_BEGIN("Editor Render");
		editor.Render();
		PROFILER_END();

		PROFILER_BEGIN("Endframe");
		graphicsEngine.EndFrame();
		PROFILER_END();
	}
}