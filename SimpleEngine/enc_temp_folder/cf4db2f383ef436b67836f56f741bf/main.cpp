#include "Engine/MemoryTracker/MemoryTracker.h" //NOTE(v9.36.4): Let this be top for now. Need to figure out a way to make sure this is called first

#include "MainSingleton/MainSingleton.hpp"
#include "Engine/Engine.hpp"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Editor/Editor.hpp"
#include "NodeScript/SimpleNodeScript.hpp"

#include "Engine/Reflection.hpp"

static void Run(HINSTANCE& hInstance, int nCmdShow);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	//SimpleTracker::SimpleMemoryTracker::StartMemoryTracking(memoryTrackerSettings); //NOTE(v9.36.4): It is inline static class wrapper now in MemoryTracker.h

	PROFILER_INIT();
	PROFILER_ENABLE();
	PROFILER_START_LISTEN();
	PROFILER_BEGIN("Main.cpp");

	Run(hInstance, nCmdShow);

	PROFILER_END();
	PROFILER_DISABLE();

	Simple::EasyProfilerOutput();

	return 0;
}

struct EmilStruct
{
	int value;
	Math::Vector3f test;
};

struct EricoStruct
{
	int value;
};

REGISTER_COMPONENT(int);
REGISTER_COMPONENT(bool);

namespace Math
{
REGISTER_COMPONENT(Vector3f);
}

REGISTER_COMPONENT(EmilStruct);
REGISTER_COMPONENT(EricoStruct);

REGISTER_PROPERTY(&ECS::NullComponent::isNull, "hello");
REGISTER_PROPERTY(&EmilStruct::value, "test");
REGISTER_PROPERTY(&EmilStruct::test, "vectorTest");

REGISTER_PROPERTY(&EricoStruct::value, "test2");

static void Run(HINSTANCE& hInstance, int nCmdShow)
{
	PROFILER_BEGIN("MainSingleton Initialize");
	MainSingleton::Init();
	PROFILER_END();

	PROFILER_BEGIN("SimpleEngine Core Class Constructors");
	Simple::Engine engine;
	Graphics::GraphicsEngine graphicsEngine;
	Editor::EditorEngine editor;
	ECS::EntityComponentSystem ecs;

	engine.SetGlobalPointerToThis();
	graphicsEngine.SetGlobalGraphicsEngineToThis();
	ecs.SetGlobalPointerToThis();
	PROFILER_END();

	PROFILER_BEGIN("SimpleEngine Core Class Initialize");
	engine.Init(hInstance, nCmdShow);
	graphicsEngine.Init(Global::GetEngineHWND(), Global::GetWindowSize());
	ecs.Init();
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

	
	/*ComponentRegistry test; test;
	
	test.RegisterComponentType<ECS::NullComponent>();
	test.RegisterComponentType<bool>();
	test.RegisterProperty(&ECS::NullComponent::isNull, "isNull");*/

	/*test.RegisterComponentType<EmilStruct>();

	test.RegisterComponentType<int>();
	*/

	/*test.RegisterProperty(&ECS::NullComponent::value, "value");
	test.RegisterProperty(&EmilStruct::value, "value");*/
	

	


	

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
		simpleScript.Update();
		editor.Update();
		PROFILER_END();

		PROFILER_BEGIN("Render To GBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::GBuffer, graphicsEngine.GetDepthBuffer().Get());
		ecs.Render();
		gameWorld.Render();
		PROFILER_END();

		PROFILER_BEGIN("Render To DeferredBuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Deferred);
		graphicsEngine.ApplyAmbientAndDirectionalLightDeferred(Graphics::eRenderTargetType::GBuffer);

		graphicsEngine.SetRenderTarget(Graphics::eRenderTargetType::Deferred, graphicsEngine.GetDepthBuffer().Get());
		ecs.RenderPointLights();
		ecs.RenderSkyBoxAndDirectionalLight();
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