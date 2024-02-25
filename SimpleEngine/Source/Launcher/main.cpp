#include "Engine/Engine.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "Engine/NoClueWhatToName/EasyProfilerOutput.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Global.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Game/GameWorld.hpp"
#include <External/imgui.h>
#include <External/TheGameAssembly/FBXImporter/source/Importer.h>

#include "Graphics/Animation/Animation.hpp"

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
		Global::SetGameIsRunning(false);
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

		TGA::FBX::Animation tgaAnimation;
		TGA::FBX::FbxImportStatus status2 = TGA::FBX::Importer::LoadAnimationA(SimpleUtilities::GetAbsolutePath("Assets/Models/Plopp_Idle.fbx"), tgaAnimation);

		Simple::Animation animation;
		animation.name = tgaAnimation.Name;
		animation.length = tgaAnimation.Length;
		animation.framesPerSecond = tgaAnimation.FramesPerSecond;
		animation.duration = static_cast<float>(tgaAnimation.Duration);
		animation.frames.resize(tgaAnimation.Frames.size());

		for (size_t i = 0; i < tgaAnimation.Frames.size(); ++i)
		{
			animation.frames[i].localTransforms.reserve(tgaAnimation.Frames[i].LocalTransforms.size());

			for (const auto& [boneName, boneTransform] : tgaAnimation.Frames[i].LocalTransforms)
			{
				Math::Matrix4x4f localMatrix;

				localMatrix(1, 1) = boneTransform.m11;
				localMatrix(1, 2) = boneTransform.m12;
				localMatrix(1, 3) = boneTransform.m13;
				localMatrix(1, 4) = boneTransform.m14;

				localMatrix(2, 1) = boneTransform.m21;
				localMatrix(2, 2) = boneTransform.m22;
				localMatrix(2, 3) = boneTransform.m23;
				localMatrix(2, 4) = boneTransform.m24;

				localMatrix(3, 1) = boneTransform.m31;
				localMatrix(3, 2) = boneTransform.m32;
				localMatrix(3, 3) = boneTransform.m33;
				localMatrix(3, 4) = boneTransform.m34;

				localMatrix(4, 1) = boneTransform.m41;
				localMatrix(4, 2) = boneTransform.m42;
				localMatrix(4, 3) = boneTransform.m43;
				localMatrix(4, 4) = boneTransform.m44;

				Math::Transform transform;
				transform.SetScale(localMatrix.GetScale());
				transform.SetRotation(localMatrix.GetRotation()); //I WILL FIX DECOMPOSE MATRIX I AM TRYING
				transform.SetPosition(localMatrix.GetPosition());

				animation.frames[i].localTransforms.emplace(boneName, transform);
			}
		}


		Simple::Model model3 = Global::GetModelFactory()->LoadFBX("Assets/Models/SK_Plopp.fbx");
		model3.SetShader("DefaultPS.cso", "AnimatedModelVS.cso");
		model3.ClearTextures();
		model3.AddTexture("Plopp.dds");
		
		Simple::AnimationPlayer animationPlayer;
		animationPlayer.Init(animation, model3);
		animationPlayer.SetIsLooping(true);
		animationPlayer.Play();

		std::shared_ptr<Simple::Model> pyramid = Global::GetModelFactory()->CreatePyramidModel();
		pyramid->SetPosition({ -2,0,0 });

		while (graphicsEngine.BeginFrame())
		{
			PROFILER_FUNCTION("MainLoop");
			engine.Update();

			//gameWorld.Update();
			//gameWorld.Render();

			Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::Backbuffer);
			Global::GetRenderer()->RenderModel(pyramid);

			//animationPlayer.Update();
			model3.SetPose(animationPlayer.myLocalSpacePose);
	
			Global::GetRenderer()->RenderModel(model3);

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