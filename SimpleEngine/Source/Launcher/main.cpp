#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/Engine.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Editor/Editor.hpp"
#include "NodeScript/SimpleNodeScript.hpp"
#include "Launcher/ErrorCatcher.hpp"

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
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("MainSingleton Initialize");
	MainSingleton::Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld Initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	PROFILER_BEGIN("SimpleScript Initialize");
	SCRIPT::SimpleNodeScript simpleScript;
	simpleScript.Init();
	PROFILER_END();

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

		PROFILER_BEGIN("Editor Update");
		simpleScript.Update();
		editor.Update();
		PROFILER_END();

		PROFILER_BEGIN("Game Update");
		gameWorld.Update();
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