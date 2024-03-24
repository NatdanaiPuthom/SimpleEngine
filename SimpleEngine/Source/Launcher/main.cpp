#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Game/World.hpp"
#include "Editor/Editor.hpp"
#include <External/imgui.h>

#include "Game/Test/ECS/ComponentManager.hpp"
#include "Game/Test/ECS/Entity.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef _SIMPLE
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 0;
#endif

	if (SimpleUtilities::InputManager::GetInstance().UpdateEvents(message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_SETCURSOR:
		::SetCursor(Global::GetCustomCursor());
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			SimpleUtilities::InputManager::GetInstance().ResetKeyStates();

			if (SimpleUtilities::InputManager::GetInstance().GetMouseIsHidden() && Global::GetGraphicsEngine()->GetCurrentCamera()->IsFreeFlyActive())
			{
				Global::GetGraphicsEngine()->GetCurrentCamera()->InactiveFreeFly();
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

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

	PROFILER_BEGIN("GameWorld");
	Simple::GameWorld gameWorld;
	ECS::ComponentManager comp;

	comp.Init();
	gameWorld.Init();
	PROFILER_END();

	struct HelloW
	{
		int a;

		~HelloW()
		{

		}
	};

	HelloW a;
	a.a = 10;

	ECS::Entity entity;
	entity.AddComponent(a);

	const auto b = entity.GetComponent<HelloW>();
	std::cout << b->a << std::endl;

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