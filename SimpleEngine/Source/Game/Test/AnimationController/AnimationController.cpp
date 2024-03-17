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

	void AnimationController::ChangeAnimation(Animation* aTargetAnimation)
	{
		if (myTargetAnimation == aTargetAnimation)
		{
			return;
		}

		myTargetAnimation = aTargetAnimation;

		myTargetAnimationPlayer.Init(*myTargetAnimation, *myAnimatedModel);
		myTargetAnimationPlayer.Play();

		myIsInterpolating = true;
	}
}