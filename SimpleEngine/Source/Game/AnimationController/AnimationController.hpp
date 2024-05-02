#pragma once

namespace Graphics
{
	struct Animation;
	class AnimatedModel;
	class AnimationPlayer;
}

namespace Simple
{
	class AnimationController final
	{
	public:
		AnimationController();
		~AnimationController();

		void Init(Graphics::AnimatedModel* aAnimatedModel, Graphics::Animation* aAnimation, const bool aShouldLoop = false);
		void Update();
		void ChangeAnimation(Graphics::Animation* aTargetAnimation, const bool aShouldLoop = false, const float aDuration = -1.0f);

	public:
		const Graphics::AnimationPlayer& GetCurrentAnimationPlayer() const;
		Graphics::AnimationPlayer& GetCurrentAnimationPlayer();

	private:
		Graphics::Animation* myCurrentAnimation;
		Graphics::Animation* myTargetAnimation;
		Graphics::AnimatedModel* myAnimatedModel;

		Graphics::AnimationPlayer myCurrentAnimationPlayer;
		Graphics::AnimationPlayer myTargetAnimationPlayer;

		float myTimer;
		float myDuration;
		bool myIsInterpolating;
	};
}