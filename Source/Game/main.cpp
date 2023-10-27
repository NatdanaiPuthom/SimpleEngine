#include "stdafx.h"
#include "Graphics/Shapes/Cube.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (SimpleUtilities::InputManager::GetInstance().UpdateEvents(message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int /*nCmdShow*/)
{
	SimpleTracker::MemoryTrackingSettings memoryTrackerSettings = {};
	memoryTrackerSettings.myShouldStoreStackTraces = false;
	memoryTrackerSettings.myShouldTrackAllAllocations = true;
	SimpleTracker::StartMemoryTracking(memoryTrackerSettings);

	{
		Engine engine;
		engine.Init(hInstance, 1280, 720);

		SimpleUtilities::InputManager::GetInstance().SetHWND(engine.GetHWND());

		Cube cube;
		cube.Create();

		while (engine.BeginFrame())
		{
			cube.Draw();
			engine.EndFrame();;
		}
	}

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here!

	SimpleTracker::StopMemoryTrackingAndPrint();
	return 0;
}
