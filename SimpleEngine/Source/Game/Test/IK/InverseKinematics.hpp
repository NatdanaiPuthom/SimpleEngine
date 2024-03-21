#pragma once

namespace Test
{
	class InverseKinematics
	{
	public:
		InverseKinematics();
		~InverseKinematics();

		void Render(std::shared_ptr< Simple::AnimatedModel> aModel, Simple::LocalSpacePose& aLocalPose);

		Math::Vector3f myKnee;
		Math::Vector3f myFoot;
	};
}