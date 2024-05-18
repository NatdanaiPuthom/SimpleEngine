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
#include "imgui.h"

static void Run(HINSTANCE& hInstance, int nCmdShow);

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

	MainSingleton::Release();

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

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
	graphicsEngine.Init(Global::GetWindowSize(), Global::GetEngineHWND());
	ecs.Init();
	editor.Init();
	PROFILER_END();

	PROFILER_BEGIN("GameWorld initialize");
	Simple::GameWorld gameWorld;
	gameWorld.Init();
	PROFILER_END();

	//Script::SimpleNodeScript simpleScript;
	//simpleScript.Init();

	Test::ShadowDSV shadowDSV = graphicsEngine.CreateShadowDSV({ 1280,720 });
	Test::ShadowDSV normalDSV = graphicsEngine.CreateShadowDSV({ 1280,720 });
	Test::ShadowRTV shadowRTV = graphicsEngine.CreateShadowRTV({ 2048,2048 }, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);

	std::shared_ptr<Graphics::Camera> shadowCamera = std::make_shared<Graphics::Camera>();
	shadowCamera->Init();

	const float shadowCameraSize = 20.0f; shadowCameraSize;

	shadowCamera->SetOrtographicProjection(shadowCameraSize, 10.0f, -1000.0f);
	graphicsEngine.SetShadowCamera(shadowCamera);

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

		{
			shadowDSV.Clear(graphicsEngine.GetContext());
			shadowRTV.Clear(graphicsEngine.GetContext());
			normalDSV.Clear(graphicsEngine.GetContext());

			graphicsEngine.SetCamera(shadowCamera);

			auto context = graphicsEngine.GetContext();
			Math::Vector4f clearColor = { 0.0f,0.0f,1.0f, 1.0f };

			ID3D11ShaderResourceView* nullSRV = nullptr;
			context->PSSetShaderResources(TEXTURE_SLOT_ALBEDO, 1, &nullSRV);
			context->OMSetRenderTargets(0, nullptr, shadowDSV.dsv.Get());
			context->ClearRenderTargetView(shadowRTV.rtv.Get(), &clearColor.x);

			ecs.Render();
			gameWorld.Render();

			if (ImGui::Begin("Shadow DSV"))
			{
				ImTextureID texture = shadowDSV.srv.Get();
				ImVec2 size = ImGui::GetWindowSize();
				ImGui::Image(texture, size);
			}
			ImGui::End();

			static bool useShadowCamera = false;

			if (MainSingleton::GetInputManager().IsKeyPressed('G'))
			{
				useShadowCamera = true;
			}
			else if (MainSingleton::GetInputManager().IsKeyPressed('F'))
			{
				useShadowCamera = false;
			}

			if (useShadowCamera)
			{
				graphicsEngine.SetCamera(shadowCamera);
			}
			else
			{
				graphicsEngine.SetCamera(graphicsEngine.GetEditorCamera());
			}
		}

		PROFILER_BEGIN("Render To ImGui");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTarget::ImGui);
		ecs.Render();
		gameWorld.Render();
		PROFILER_END();

		PROFILER_BEGIN("Render To Backbuffer");
		graphicsEngine.SetRenderTarget(Graphics::eRenderTarget::Backbuffer);
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