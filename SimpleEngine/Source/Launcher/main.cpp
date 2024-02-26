#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include <External/imgui.h>

#include "Game/Test/ECS.hpp"

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

struct HelloWorld
{
	int a = 5;
};

namespace Simple
{
	class PlayerSystem final : private System
	{
	public:
		void Update() override
		{
			for (std::size_t i = 0; i < myEntities.size(); ++i)
			{
				std::cout << myEntities[i].GetComponent<HelloWorld>()->a << std::endl;
			}
		};

		void Render() override {};

		std::vector<Entity> myEntities;
	};
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

		PROFILER_BEGIN("Constructor");
		Simple::Engine engine;
		Simple::GraphicsEngine graphicsEngine;
		PROFILER_END();

		PROFILER_BEGIN("SetGlobalPointers");
		engine.SetGlobalPointerToThis();
		graphicsEngine.SetGlobalGraphicsEngineToThis();
		PROFILER_END();

		PROFILER_BEGIN("Inits");
		engine.Init(hInstance, nCmdShow);
		graphicsEngine.Init(Global::GetWindowSize(), Global::GetEngineHWND());
		PROFILER_END();

		SimpleUtilities::InputManager::GetInstance().SetHWND(Global::GetEngineHWND());

		PROFILER_BEGIN("GameWorld");
		Simple::GameWorld gameWorld;
		gameWorld.Init();
		PROFILER_END();

		Simple::ComponentManager componentManager;
		componentManager.SetWorldPointerToThis();

		Simple::Entity entity;
		entity.AddComponent(HelloWorld());
		std::cout << entity.GetComponent<HelloWorld>()->a;


		//Simple::PlayerSystem playerSystem;
		//playerSystem.myEntities.push_back(entity);
		//playerSystem.Update();

		//std::cout << componentManager.GetAllComponentsOfType<HelloWorld>().size() << std::endl;


		while (graphicsEngine.BeginFrame())
		{
			PROFILER_FUNCTION("MainLoop");

			engine.Update();

			if (graphicsEngine.GetCurrentCamera() == graphicsEngine.GetDefaultCamera())
			{
				graphicsEngine.GetDefaultCamera()->Update(engine.GetDeltaTime());
			}

			gameWorld.Update();

			gameWorld.Render();

			graphicsEngine.EndFrame();;
		}
	}

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}