#include "Game/Precomplied/GamePch.hpp"
#include "Test/IK/InverseKinematics.hpp"
#include <cmath>

namespace Test
{
	static double CalculateTheta0(double l0, double l1, double m, double E_angle)
	{
		return acos((pow(l0, 2) - pow(l1, 2) + m) / (2 * l0 * sqrt(m))) + E_angle;
	}

	static double CalculateTheta1(double l0, double l1, double m)
	{
		return acos((pow(l0, 2) + pow(l1, 2) - m) / (2 * l0 * l1));
	}

	InverseKinematics::InverseKinematics()
	{
	}

	InverseKinematics::~InverseKinematics()
	{
	}

	void InverseKinematics::Render(std::shared_ptr< Simple::AnimatedModel> aModel, Simple::LocalSpacePose& aLocalPose)
	{
		auto hipPosition = aLocalPose.jointTransforms[39].GetPosition();
		auto kneePosition = aLocalPose.jointTransforms[40].GetPosition();
		auto footPosition = aLocalPose.jointTransforms[41].GetPosition();

		if (ImGui::Begin("Skeletons"))
		{
			if (ImGui::DragFloat3("Hip", &hipPosition.x, 0.1f, -180.0f, 180.0f))
			{
				aLocalPose.jointTransforms[39].SetPosition(hipPosition);
				aModel->SetPose(aLocalPose);
			}

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
		}
		ImGui::End();

		auto renderer = Global::GetRenderer();

		Simple::ModelSpacePose pose;
		const Simple::Skeleton* skeleton = aModel->GetSkeleton();
		skeleton->ConvertPoseToModelSpace(aLocalPose, pose);

		const Math::Matrix4x4f modelTransform = aModel->GetMatrix();

		const Math::Matrix4x4 hip = pose.jointTransforms[39] * modelTransform;
		const Math::Matrix4x4 knee = pose.jointTransforms[40] * modelTransform;
		const Math::Matrix4x4 fot = pose.jointTransforms[41] * modelTransform;

		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = knee.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.1f;

			renderer->RenderLine(line);
		}

		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = fot.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.1f;

			renderer->RenderLine(line);
		}

		{
			Drawer::Line line;
			line.color = { 0.0f, 0.0f, 1.0f, 1.0f };
			line.startPosition = hip.GetPosition();
			line.endPosition = line.startPosition;
			line.endPosition.y -= 0.1f;

			renderer->RenderLine(line);
		}

		renderer->RenderAnimatedSkeletonLines(aModel, aLocalPose);
		renderer->RenderLine(myLines);

		Test();
	}

	void InverseKinematics::Test()
	{
		auto renderer = Global::GetRenderer();

		static float l0 = 10.0f;
		static float l1 = 10.0f;
		static float E1 = 20.0f;
		static float E2 = 2.0f;
		static float B1 = 8.0f;
		static float B2 = 15.0f;

		if (ImGui::Begin("Test"))
		{
			ImGui::DragFloat("l0", &l0);
			ImGui::DragFloat("l1", &l1);
			ImGui::DragFloat("E1", &E1);
			ImGui::DragFloat("E2", &E2);
			ImGui::DragFloat("B1", &B1);
			ImGui::DragFloat("B2", &B2);
		}
		ImGui::End();

		// Intermediate calculations
		float m = powf((E1 - B1), 2) + powf((E2 - B2), 2);
		float E_angle = atan2f(E2 - B2, E1 - B1);
		float d = Math::GetMax(fabsf(l0 - l1), Math::GetMin(l0 + l1, sqrtf(powf(E1 - B1, 2) + powf(E2 - B2, 2))));

		// Calculate theta0
		double theta0 = CalculateTheta0(l0, l1, m, E_angle);

		// Calculate theta1
		double theta1 = CalculateTheta1(l0, l1, m);

		// Convert theta1 to degrees
		//double theta_d = theta1 * 180 / Math::globalPi;

		double SE_x = cos(E_angle) * d;
		double SE_y = sin(E_angle) * d;
		double S0_x = B1 + cos(theta0) * l0;
		double S0_y = B2 + sin(theta0) * l0;
		double S1_x = S0_x + cos(Math::globalPi - (-theta1 - theta0)) * l1;
		double S1_y = S0_y + sin(Math::globalPi - (-theta1 - theta0)) * l1;

		S1_y; S1_x; S0_y; S0_x;

		Drawer::Line line;
		line.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		line.startPosition = { static_cast<float>(SE_x), static_cast<float>(SE_y), 0.0f };
		line.endPosition = { static_cast<float>(S0_x), static_cast<float>(S0_y), 0.0f };
		renderer->RenderLine(line);

		line.startPosition = { static_cast<float>(S0_x), static_cast<float>(S0_y), 0.0f };
		line.endPosition = { static_cast<float>(S1_x), static_cast<float>(S1_y), 0.0f };;
		renderer->RenderLine(line);

		Drawer::Sphere sphere;
		sphere.radius = 1.0f;

		sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		sphere.position = { static_cast<float>(SE_x), static_cast<float>(SE_y), 0.0f };
		renderer->RenderSphere(sphere);

		sphere.color = { 0.0f, 1.0f, 0.0f, 1.0f };
		sphere.position = { static_cast<float>(S0_x), static_cast<float>(S0_y), 0.0f };
		renderer->RenderSphere(sphere);

		sphere.color = { 0.0f, 0.0f, 1.0f, 1.0f };
		sphere.position = { static_cast<float>(S1_x), static_cast<float>(S1_y), 0.0f };
		renderer->RenderSphere(sphere);
	}
}