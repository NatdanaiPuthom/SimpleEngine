#include "Game/Precomplied/GamePch.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myLevelManager(std::make_unique<Simple::LevelManager>())
		, myImGuiManager(std::make_unique<Simple::ImGuiManager>())
		, myRaycastManager(std::make_unique<Simple::RaycastManager>())
		, myEventManager(std::make_unique<Simple::EventManager>())
		, myFixedUpdateTimer(0.0f)
		, myFixedUpdateFreqency(1.0f / 60.0f)
	{
		Impl::SimpleWorldGameWorld::SetGameWorld(this);
	}

	GameWorld::~GameWorld()
	{
	}

	void GameWorld::Init()
	{
		myImGuiManager->Init();
		myLevelManager->Init();
		myRaycastManager->Init();

		myAnimationTest.model = Global::GetModelFactory()->LoadMeshFBX("Assets/Models/SimpleCube_WithArm.fbx");
		myAnimationTest.animation = Global::GetModelFactory()->LoadAnimationFBX("Assets/Models/A_SimpleCube_WithArm_Idle.fbx");

		myAnimationTest.model.SetShader("DefaultPS.cso", "AnimatedModelVS.cso");

		myAnimationTest.animationPlayer.Init(myAnimationTest.animation, myAnimationTest.model);
		myAnimationTest.animationPlayer.SetIsLooping(true);
		myAnimationTest.animationPlayer.Play();
	}

	void GameWorld::Update()
	{
		EarlyUpdate();

		FixedUpdate();

		NormalUpdate();

		LateUpdate();
	}

	void RenderSkeletonLines(const Simple::Model& aModel, const Simple::LocalSpacePose& aLocalPose)
	{
		std::vector<Drawer::Line> lines;
		lines.reserve(aLocalPose.count);

		Simple::ModelSpacePose pose;
		aModel.myMesh->mySkeleton.ConvertPoseToModelSpace(aLocalPose, pose);

		const Math::Matrix4x4f modelTransform = aModel.GetMatrix();

		for (size_t index = 0; index < aLocalPose.count; ++index)
		{
			Simple::Joint joint = aModel.myMesh->mySkeleton.myJoints[index];

			if (joint.myParent == -1)
				continue;

			//const Math::Matrix4x4 boneWorldTransform = Math::Matrix4x4f::GetInverse(aLocalPose.jointTransforms[index]);
			//const Math::Matrix4x4 boneWorldTransformNext = Math::Matrix4x4f::GetInverse(aLocalPose.jointTransforms[joint.myParent]);

			const Math::Matrix4x4 boneWorldTransform = Math::Matrix4x4f::GetInverse(pose.jointTransforms[index]) * modelTransform;
			const Math::Matrix4x4 boneWorldTransformNext = Math::Matrix4x4f::GetInverse(pose.jointTransforms[joint.myParent]) * modelTransform;

			Drawer::Line line;
			line.startPosition = boneWorldTransform.GetPosition();
			line.endPosition = boneWorldTransformNext.GetPosition();
			lines.push_back(line);
		}

		Global::GetRenderer()->RenderLineInstance(lines);
	}

	void GameWorld::Render()
	{
		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::WaterReflection);
		Global::GetGraphicsEngine()->GetRenderer()->RenderEverythingUpSideDown();

		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::WaterRefraction);
		Global::GetGraphicsEngine()->GetRenderer()->RenderRefraction();

		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::Backbuffer);

		myLevelManager->Render();
		myRaycastManager->Render();

		myAnimationTest.animationPlayer.Update();
		myAnimationTest.model.SetPose(myAnimationTest.animationPlayer.myLocalSpacePose);

		RenderSkeletonLines(myAnimationTest.model, myAnimationTest.animationPlayer.myLocalSpacePose);
	//Global::GetRenderer()->RenderModel(myAnimationTest.model);

		myImGuiManager->Render();
	}

	void GameWorld::NormalUpdate()
	{
		myLevelManager->Update();
		myRaycastManager->Update();
	}

	void GameWorld::EarlyUpdate()
	{
	}

	void GameWorld::FixedUpdate()
	{
		myFixedUpdateTimer += Global::GetDeltaTime();

		if (myFixedUpdateTimer < myFixedUpdateFreqency)
		{
			return;
		}

		myFixedUpdateTimer = 0.0f;
	}

	void GameWorld::LateUpdate()
	{
	}
}