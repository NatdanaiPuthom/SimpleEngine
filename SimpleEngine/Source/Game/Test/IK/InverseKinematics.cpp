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
		Math::Vector3f jointRotation;
		Math::Vector3f jointScale;
		std::string selectedJointName = "Selected: None";

		if (mySelectedJoint != nullptr)
		{
			jointPosition = mySelectedJoint->myBindPoseInverse.GetPosition();
			jointRotation = mySelectedJoint->myBindPoseInverse.GetEulerRotationInDegree();
			jointScale = mySelectedJoint->myBindPoseInverse.GetScale();
			selectedJointName = "Selected: " + mySelectedJoint->myName;
		}

		ImGui::Dummy(ImVec2(0, 25));
		ImGui::Text(selectedJointName.c_str());

		if (ImGui::DragFloat3("Position", &jointPosition.x, 0.1f))
		{
			if (mySelectedJoint != nullptr)
			{
				mySelectedJoint->myBindPoseInverse.SetPosition(jointPosition);
			}
		}

		if (ImGui::DragFloat3("Rotation", &jointRotation.x, 0.1f))
		{
			if (mySelectedJoint != nullptr)
			{
				mySelectedJoint->myBindPoseInverse.SetLocalRotation(jointRotation);
			}
		}

		if (ImGui::DragFloat3("Scale", &jointScale.x, 0.1f))
		{
			if (mySelectedJoint != nullptr)
			{
				mySelectedJoint->myBindPoseInverse.SetScale(jointScale);
			}
		}

		ImGui::End();
	}

	void InverseKinematics::DisplayName(std::vector<Simple::Joint>& aOriginalJoints, Simple::Joint& aJoint)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen;

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