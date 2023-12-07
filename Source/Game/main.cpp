#include "Game/GameWorld.h"
#include "Engine/engine.h"
#include "Engine/Input/InputManager.h"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include <External/profiler.h>
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
		SimpleUtilities::InputManager::GetInstance().ResetKeyStates();
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

		Engine engine;
		engine.Init(hInstance, nCmdShow);

		SimpleUtilities::InputManager::GetInstance().SetHWND(engine.GetHWND());

		GameWorld gameWorld;

		while (engine.BeginFrame())
		{
			gameWorld.Render();
			engine.EndFrame();;
		}
	}

	PROFILER_END();
	PROFILER_DISABLE();

	{
		const std::string output = SIMPLE_PROFILER_DIR + std::string(SIMPLE_PROFILER_FILENAME);
		PROFILER_DUMP_FILE(output.c_str());
	}

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}
