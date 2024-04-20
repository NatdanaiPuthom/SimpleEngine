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
		ImGui::End();

		const auto renderer = Global::GetRenderer();

		renderer->RenderModel(myTestModel);
		//renderer->RenderAnimatedSkeletonLines(myTestModel, myTestAnimationPlayer.myModelSpacePose);
		renderer->TestIKSkeletonLines(myTestModel);

		if (mySelectedJoint != nullptr)
		{
			Simple::Joint* test = const_cast<Simple::Joint*>(mySelectedJoint);

			auto pos = mySelectedJoint->myBindPoseInverse.GetPosition();
			pos.x += 1 * Global::GetDeltaTime();
			test->myBindPoseInverse.SetPosition(pos);
		}
	}

	void InverseKinematics::DisplayName(const std::vector<Simple::Joint>& aOriginalJoints, const Simple::Joint& aJoint)
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
			for (auto a : aJoint.myChildren)
			{
				DisplayName(aOriginalJoints, aOriginalJoints[a]);
			}
			ImGui::TreePop();
		}
	}
}