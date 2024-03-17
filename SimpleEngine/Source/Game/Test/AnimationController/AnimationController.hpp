#pragma once

namespace Simple
{
	class AnimationController final
	{
	public:
		AnimationController();
		~AnimationController();

		void Init(AnimatedModel* aAnimatedModel, Animation* aAnimation, const bool aShouldLoop = false);
		void Update();
		void ChangeAnimation(Animation* aTargetAnimation);
	private:
		Animation* myCurrentAnimation;
		Animation* myTargetAnimation;
		AnimatedModel* myAnimatedModel;

		AnimationPlayer myCurrentAnimationPlayer;
		AnimationPlayer myTargetAnimationPlayer;

		float myTimer;
		float myDuration;
		bool myIsInterpolating;
	};
}