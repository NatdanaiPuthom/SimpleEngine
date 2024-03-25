#pragma once

namespace Test
{
	class InverseKinematics
	{
	public:
		InverseKinematics();
		~InverseKinematics();

		void Render(std::shared_ptr< Simple::AnimatedModel> aModel, Simple::LocalSpacePose& aLocalPose);

		std::vector<Drawer::Line> myLines;

	private:
		void Test();
		void Arm();
	};
}