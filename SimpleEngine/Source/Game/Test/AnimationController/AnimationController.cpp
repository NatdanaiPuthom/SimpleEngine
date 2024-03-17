#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/AnimationController/AnimationController.hpp"

namespace Simple
{

	AnimationController::AnimationController()
		: myIsInterpolating(false)
		, myDuration(0.0f)
		, myTimer(0.0f)
		, myCurrentAnimation(nullptr)
		, myTargetAnimation(nullptr)
		, myAnimatedModel(nullptr)
	{
	}

	AnimationController::~AnimationController()
	{
	}

	void AnimationController::Init(AnimatedModel* aAnimatedModel, Animation* aAnimation, const bool aShouldLoop)
	{
		myAnimatedModel = aAnimatedModel;
		myCurrentAnimation = aAnimation;

		myCurrentAnimationPlayer.Init(*myCurrentAnimation, *myAnimatedModel);
		myCurrentAnimationPlayer.SetIsLooping(aShouldLoop);
		myCurrentAnimationPlayer.Play();
	}

	void AnimationController::Update()
	{
		myCurrentAnimationPlayer.Update();
	}

	void AnimationController::ChangeAnimation(Animation* aTargetAnimation, const bool aShouldLoop, const float aDuration)
	{
		if (myTargetAnimation == aTargetAnimation)
		{
			return;
		}

		myDuration = myCurrentAnimation->duration - myCurrentAnimationPlayer.GetTime();

		if (aDuration > myDuration)
		{
			myDuration = aDuration;
		}

		myTargetAnimation = aTargetAnimation;

		myTargetAnimationPlayer.Init(*myTargetAnimation, *myAnimatedModel);
		myTargetAnimationPlayer.SetIsLooping(aShouldLoop);
		myTargetAnimationPlayer.Restart();

		myIsInterpolating = true;
	}
}