#pragma once

namespace Test
{
	class InverseKinematics
	{
	public:
		InverseKinematics();
		~InverseKinematics();

		void Init();

		void Update();
		void Render();

	private:
		void DisplayName(std::vector<Simple::Joint>& aOriginalJoints, Simple::Joint& aJoint);

	private:
		Simple::Animation myTestAnimation;
		Simple::AnimatedModel myTestModel;
		Simple::AnimationPlayer myTestAnimationPlayer;

		Simple::Joint* mySelectedJoint;
	};
}