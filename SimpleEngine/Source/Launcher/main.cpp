#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include <External/imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 0;

	if (SimpleUtilities::InputManager::GetInstance().UpdateEvents(message, wParam, lParam))
		return 0;

	switch (message)
	{
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

	{
		PROFILER_FUNCTION(profiler::colors::Blue);
		PROFILER_BEGIN("Engine initialize");
		Simple::Engine engine;
		Simple::GraphicsEngine graphicsEngine;

		engine.SetGlobalPointerToThis();
		graphicsEngine.SetGlobalGraphicsEngineToThis();

		engine.Init(hInstance, nCmdShow);
		graphicsEngine.Init(Global::GetWindowSize(), Global::GetEngineHWND());
		AudioManager::GetInstance().Init();

		SimpleUtilities::InputManager::GetInstance().SetHWND(Global::GetEngineHWND());
		PROFILER_END();

		PROFILER_BEGIN("GameWorld");
		Simple::GameWorld gameWorld;
		gameWorld.Init();
		PROFILER_END();

		while (Global::GetGameIsRunning())
		{
			PROFILER_BEGIN("Frame Time");
			
			graphicsEngine.BeginFrame();

			engine.Update();
			graphicsEngine.GetDefaultCamera()->Update(engine.GetDeltaTime()); //For now only 1 Camera (v9.18.0)

			gameWorld.Update();
			gameWorld.Render();

			graphicsEngine.EndFrame();
			PROFILER_END()
		}
	}

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	AudioManager::GetInstance().~AudioManager(); //I will fix so AudioManager isn't a singleton later (v9.18.0)

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}