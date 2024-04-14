#include "Game/Precomplied/GamePch.hpp"
#include "Test/IK/InverseKinematics.hpp"

namespace Test
{
	InverseKinematics::InverseKinematics()
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
	}

	void InverseKinematics::Render()
	{
		const std::vector<Simple::Joint>& joints = myTestModel.GetSkeleton()->myJoints;

		Simple::Joint root;

		for (const auto& joint : joints)
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
		renderer->RenderStaticSkeletonLines(myTestModel);
		//renderer->RenderAnimatedSkeletonLines(myTestModel, myTestAnimationPlayer.myLocalSpacePose);
	}

	void InverseKinematics::DisplayName(const std::vector<Simple::Joint>& aOriginalJoints, const Simple::Joint& aJoint)
	{
		if (aJoint.myChildren.empty())
		{
			ImGui::BulletText("%s", aJoint.myName.c_str());
		}
		else
		{
			if (ImGui::TreeNode(aJoint.myName.c_str()))
			{
				for (auto a : aJoint.myChildren)
				{
					DisplayName(aOriginalJoints, aOriginalJoints[a]);
				}

				ImGui::TreePop();
			}
		}
	}
}