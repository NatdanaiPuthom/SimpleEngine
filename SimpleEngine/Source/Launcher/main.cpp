#define WIN32_LEAN_AND_MEAN //NOTE(v11.2.1): Fuck you Microsoft

#include "Engine/Debugger/MemoryTracker/MemoryTracker.h"
#include "Engine/Engine.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Debugger/RuntimeCrashCatcher/RuntimeCrashCatcher.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Editor/Editor.hpp"
#include "NodeScript/SimpleNodeScript.hpp"

//#include "Launcher/Client/Client.hpp"

static void Run(HINSTANCE& hInstance, int nCmdShow);
static void RunWithSEH(HINSTANCE& hInstance, int nCmdShow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	PROFILER_INIT();
	PROFILER_ENABLE();
	PROFILER_START_LISTEN();
	PROFILER_BEGIN("Main.cpp");

	RunWithSEH(hInstance, nCmdShow);

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	MainSingleton::Destroy();

	return 0;
}

static void RunWithSEH(HINSTANCE& hInstance, int nCmdShow)
{
	__try
	{
		Run(hInstance, nCmdShow);
	}
	__except (ExceptionFilter(GetExceptionInformation()))
	{
	}
}

static void Run(HINSTANCE& hInstance, int nCmdShow)
{
	PROFILER_BEGIN("SimpleEngine Core Class Constructors");
	Simple::Engine engine;
	Graphics::GraphicsEngine graphicsEngine;
	Editor::EditorEngine editor;

	engine.SetGlobalPointerToThis();
	graphicsEngine.SetGlobalGraphicsEngineToThis();
	PROFILER_END();

	PROFILER_BEGIN("SimpleEngine Core Class Initialize");
	engine.Init(hInstance, nCmdShow);
	graphicsEngine.Init(Global::GetEngineHWND(), Global::GetWindowSize());
	MainSingleton::Init(); //NOTE(v11.3.2): Fix dependency where Editor crashes if init before and engine crash if init after MainSingleton
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld Initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	PROFILER_BEGIN("SimpleScript Initialize");
	SCRIPT::SimpleNodeScript simpleScript;
	simpleScript.Init();
	PROFILER_END();

	/*Simple::Client client;
	if (client.Init() == false)
	{
		assert(false && "Failed to connect. The program will now close.");
		return;
	}*/

	while (const bool isRunning = Global::GetGameIsRunning())
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

		/*if (client.Update(isRunning) == false)
		{
			Global::SetGameShouldClose(true);
		}*/

		PROFILER_BEGIN("Editor Update");
		editor.Update();
		PROFILER_END();

		PROFILER_BEGIN("Game Update");
		gameWorld.EarlyUpdate();
		gameWorld.FixedUpdate();
		gameWorld.Update();
		gameWorld.LateUpdate();
		PROFILER_END();

		PROFILER_BEGIN("Render To GBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::GBuffer, graphicsEngine.GetDepthBuffer().Get());
		gameWorld.Render();
		PROFILER_END();

		PROFILER_BEGIN("Render To DeferredBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Deferred);
		graphicsEngine.ApplyAmbientAndDirectionalLightDeferred(Graphics::eRenderTargetType::GBuffer);

		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Deferred, graphicsEngine.GetDepthBuffer().Get());
		gameWorld.LateRender();
		PROFILER_END();

		PROFILER_BEGIN("Render To BloomRenderTarget");
		graphicsEngine.ApplyBloom();
		PROFILER_END();

		PROFILER_BEGIN("Render To PostProcessingBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::PostProcessing);
		graphicsEngine.ApplyPostProcessing(Graphics::eRenderTargetType::Bloom);
		PROFILER_END();

		PROFILER_BEGIN("Render to BackBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Backbuffer);
		graphicsEngine.RenderFullScreenCopy(Graphics::eRenderTargetType::PostProcessing);
		editor.Render();
		PROFILER_END();

		PROFILER_BEGIN("Endframe");
		graphicsEngine.EndFrame();
		PROFILER_END();
	}
}