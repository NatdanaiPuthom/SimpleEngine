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
		void DisplayName(const std::vector<Simple::Joint>& aOriginalJoints, const Simple::Joint& aJoint);

	private:
		Simple::Animation myTestAnimation;
		Simple::AnimatedModel myTestModel;
		Simple::AnimationPlayer myTestAnimationPlayer;

		const Simple::Joint* mySelectedJoint;
	};
}