#include "stdafx.h"
#include "Engine/Graphics/Model/Model.h"
#include "Engine/Graphics/Shapes/Cube.h"
#include "Engine/Graphics/Shapes/Pyramid.h"
#include <External/profiler.h>

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
		engine.Init(hInstance, 1280, 720, nCmdShow);

		SimpleUtilities::InputManager::GetInstance().SetHWND(engine.GetHWND());

		std::vector<std::unique_ptr<Model>> models;
		models.emplace_back(std::make_unique<Cube>());
		models.emplace_back(std::make_unique<Pyramid>());

		for (auto& model : models)
		{
			if (model->Create() == false)
				assert(false && "Failed to create model");
		}

		while (engine.BeginFrame())
		{
			engine.Render();

			for (const auto& model : models)
			{
				model->Draw();
			}

			engine.EndFrame();;
		}
	}

	PROFILER_END();
	PROFILER_DISABLE();

	const std::string output = SIMPLE_PROFILER_DIR + std::string("profiler_data.prof");
	PROFILER_DUMP_FILE(output.c_str());

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}
