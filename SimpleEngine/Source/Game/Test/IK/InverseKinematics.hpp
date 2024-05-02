#pragma once
#include <vector>

namespace Graphics
{
	struct Animation;
	class Joint;
	class AnimatedModel;
	class AnimationPlayer;
}

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
		void DisplayName(std::vector<Graphics::Joint>& aOriginalJoints, Graphics::Joint& aJoint);

	private:
		Graphics::Animation myTestAnimation;
		Graphics::AnimatedModel myTestModel;
		Graphics::AnimationPlayer myTestAnimationPlayer;

		Graphics::Joint* mySelectedJoint;
	};
}