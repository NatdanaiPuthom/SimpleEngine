#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include "Editor/Editor.hpp"

#include "NodeScript/SimpleNodeScript.hpp"

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

	//Remember to release any allocated memory from static classes/variables to avoid false memory leaks!
	//As I have no clue how to call StopMemoryTracking AFTER all static classes call their destructor, so do it here before StopMemoryTrackingAndPrint function!
	SimpleTracker::StopMemoryTrackingAndPrint();

	return 0;
}

static void Run(HINSTANCE& hInstance, int nCmdShow)
{
	PROFILER_BEGIN("Engine initialize");
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

	ECS::Entity entity = ecs.CreateEntity();
	entity->AddComponent<ECS::MeshComponent>();
	entity->AddComponent<ECS::TransformComponent>();
	entity->AddComponent<ECS::AnimatedComponent>();
	entity->AddComponent<ECS::AnimationPlayerComponent>();

	{
		ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
		ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();
		ECS::AnimationPlayerComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationPlayerComponent>();

		Graphics::ModelFactory* modelFactory = Global::GetModelFactory();

		meshComponent->mesh = modelFactory->LoadMesh("AnimatedModels/SimpleHuman3.fbx");
		meshComponent->shader = graphicsEngine.GetDefaultShader().get();
		meshComponent->texture = graphicsEngine.GetDefaultTexture().get();

		animatedComponent->shader = graphicsEngine.GetDefaultAnimatedShader().get();
		animatedComponent->skeleton = modelFactory->LoadSkeleton("AnimatedModels/SimpleHuman3.fbx");
		animatedComponent->animation = modelFactory->LoadAnimationFBX("Animations/SimpleHuman3_Idle.fbx");

		animationPlayerComponent->animationPlayer.Init(animatedComponent->animation, animatedComponent->skeleton);
		animationPlayerComponent->animationPlayer.Play(true);
	}

	Script::SimpleNodeScript simpleScript;
	simpleScript.Init();

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

		PROFILER_BEGIN("GameWorld Update");
		gameWorld.Update();
		PROFILER_END();

		PROFILER_BEGIN("Editor Update");
		simpleScript.Update();
		editor.Update();
		PROFILER_END();

		PROFILER_BEGIN("SetRenderTarget: Backbuffer");
		graphicsEngine.SetRenderTarget(eRenderTarget::Backbuffer);
		PROFILER_END();



		if (entity != nullptr)
		{
			ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = entity->GetComponent<ECS::TransformComponent>();
			ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();
			ECS::AnimationPlayerComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationPlayerComponent>();

			animationPlayerComponent->animationPlayer.UpdateTest(animatedComponent->jointMatrices, animatedComponent);
			Global::GetRenderer()->RenderAnimatedModel(transformComponent, meshComponent, animatedComponent);
		}




		PROFILER_BEGIN("GameWorld Render");
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