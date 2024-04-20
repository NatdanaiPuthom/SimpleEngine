#include "Game/Precomplied/GamePch.hpp"
#include "Test/IK/InverseKinematics.hpp"

namespace Test
{
	InverseKinematics::InverseKinematics()
		: mySelectedJoint(nullptr)
	{
	}

	InverseKinematics::~InverseKinematics()
	{
	}

	void InverseKinematics::Init()
	{
		myTestModel = Global::GetModelFactory()->LoadAnimatedModelFBX("AnimatedModels/SimpleHuman3.fbx");;
		myTestAnimation = Global::GetModelFactory()->LoadAnimationFBX("Animations/SimpleHuman3_Idle.fbx");
		myTestAnimationPlayer.Init(myTestAnimation, myTestModel);
		myTestAnimationPlayer.Play(true);

		Global::GetGraphicsEngine()->SetRasterizerState(eRasterizerState::Wireframe);
	}

	void InverseKinematics::Update()
	{
		//myTestAnimationPlayer.Update();

		/*auto pos = myTestAnimationPlayer.myLocalSpacePose.jointTransforms[3].GetPosition();
		pos.x += 0.1f * Global::GetDeltaTime();
		myTestAnimationPlayer.myLocalSpacePose.jointTransforms[3].SetPosition(pos);
		myTestModel.SetPose(myTestAnimationPlayer.myLocalSpacePose);*/
	}

	void InverseKinematics::Render()
	{
		const auto renderer = Global::GetRenderer();

		renderer->RenderModel(myTestModel);
		//renderer->RenderAnimatedSkeletonLines(myTestModel, myTestAnimationPlayer.myModelSpacePose);
		renderer->TestIKSkeletonLines(myTestModel);

		std::vector<Simple::Joint>& joints = myTestModel.GetTestIKSkeleton().myJoints;
		Simple::Joint root;

		for (auto& joint : joints)
		{
			if (joint.myParent == -1)
			{
				root = joint;
			}
		}

		if (ImGui::Begin("Joints"))
		{
			DisplayName(joints, root);
		}

		Math::Vector3f jointPosition;

		if (mySelectedJoint != nullptr)
		{
			jointPosition = mySelectedJoint->myBindPoseInverse.GetPosition();
		}

		if (ImGui::DragFloat3("Test", &jointPosition.x, 0.1f))
		{
			mySelectedJoint->myBindPoseInverse.SetPosition(jointPosition);
		}

		ImGui::End();
	}

	void InverseKinematics::DisplayName(std::vector<Simple::Joint>& aOriginalJoints, Simple::Joint& aJoint)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (aJoint.myChildren.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (&aJoint == mySelectedJoint)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		const bool open = ImGui::TreeNodeEx(aJoint.myName.c_str(), flags);

		if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			mySelectedJoint = &aJoint;
		}

		if (open)
		{
			for (auto child : aJoint.myChildren)
			{
				DisplayName(aOriginalJoints, aOriginalJoints[child]);
			}
			ImGui::TreePop();
		}
	}
}