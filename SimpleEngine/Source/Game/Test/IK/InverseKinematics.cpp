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

	void InverseKinematics::Render(std::shared_ptr< Simple::AnimatedModel> aModel, Simple::LocalSpacePose& aLocalPose)
	{
		auto kneePosition = aLocalPose.jointTransforms[39].GetPosition();
		auto footPosition = aLocalPose.jointTransforms[40].GetPosition();
		auto toePosition = aLocalPose.jointTransforms[41].GetPosition();


		auto footRotation = aLocalPose.jointTransforms[42].GetEulerRotationInDegree();

		if (ImGui::Begin("Skeletons"))
		{
			if (ImGui::DragFloat3("Knee", &kneePosition.x, 0.1f, -180.0f, 180.0f))
			{
				aLocalPose.jointTransforms[40].SetPosition(kneePosition);
				aModel->SetPose(aLocalPose);
			}

			if (ImGui::DragFloat3("Foot", &footPosition.x, 0.1f, -180.0f, 180.0f))
			{
				aLocalPose.jointTransforms[41].SetPosition(footPosition);
				aModel->SetPose(aLocalPose);
			}

			if (ImGui::DragFloat3("Toe", &toePosition.x, 0.1f, -180.0f, 180.0f))
			{
				aLocalPose.jointTransforms[42].SetPosition(toePosition);
				aModel->SetPose(aLocalPose);
			}
		}
		ImGui::End();

		Simple::ModelSpacePose pose;
		const Simple::Skeleton* skeleton = aModel->GetSkeleton();
		skeleton->ConvertPoseToModelSpace(aLocalPose, pose);

		const Math::Matrix4x4f modelTransform = aModel->GetMatrix();

		const Math::Matrix4x4 knee = pose.jointTransforms[40] * modelTransform;
		const Math::Matrix4x4 fot = pose.jointTransforms[41] * modelTransform;
		const Math::Matrix4x4 toe = pose.jointTransforms[42] * modelTransform;


		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = knee.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.4f;

			Global::GetRenderer()->RenderLine(line);
		}

		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = fot.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.4f;

			Global::GetRenderer()->RenderLine(line);
		}

		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = toe.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.4f;

			Global::GetRenderer()->RenderLine(line);
		}
	}
}