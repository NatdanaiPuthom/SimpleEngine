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
		if (myIsInterpolating == true)
		{
			myTimer += Global::GetDeltaTime();

			myCurrentAnimationPlayer.LerpCurrentAnimation();
			myTargetAnimationPlayer.LerpCurrentAnimation();

			const float t = std::clamp<float>(myTimer / myDuration, 0.0f, 1.0f);
			auto skeleton = myAnimatedModel->GetSkeleton();

			for (size_t i = 0; i < skeleton->myJoints.size(); i++)
			{
				const Math::Matrix4x4f& currentFrameJointTransform = myCurrentAnimationPlayer.GetLocalSpacePose().jointTransforms[i];
				const Math::Matrix4x4f& targetFrameJointTransform = myTargetAnimationPlayer.GetLocalSpacePose().jointTransforms[i];

				Math::Vector3 currentPosition;
				Math::Vector3 targetPosition;

				Math::Quaternion currentQuaternion;
				Math::Quaternion targetQuaternion;

				Math::Vector3 currentScale;
				Math::Vector3 targetScale;

				currentFrameJointTransform.DecomposeMatrix(currentPosition, currentQuaternion, currentScale);
				targetFrameJointTransform.DecomposeMatrix(targetPosition, targetQuaternion, targetScale);

				const Math::Vector3 translation = Math::Lerp(currentPosition, targetPosition, t);
				const Math::Quaternionf rotation = Math::Quaternionf::Slerp(currentQuaternion, targetQuaternion, t);
				const Math::Vector3 scale = Math::Lerp(currentScale, targetScale, t);

				const Math::Matrix4x4f Result = Math::Matrix4x4f::CreateScaleMatrix(scale) * rotation.GetRotationMatrix4x4() * Math::Matrix4x4f::CreateTranslationMatrix(translation);
				myCurrentAnimationPlayer.myLocalSpacePose.jointTransforms[i] = Result;
			}

			myCurrentAnimationPlayer.myLocalSpacePose.count = skeleton->myJoints.size();
			myAnimatedModel->SetPose(myCurrentAnimationPlayer.myLocalSpacePose);

			if (t >= 1.0f)
			{
				myIsInterpolating = false;
				myCurrentAnimation = myTargetAnimation;
				myCurrentAnimationPlayer.Init(*myCurrentAnimation, *myAnimatedModel);
			}
		}
		else
		{
			myCurrentAnimationPlayer.Update();
		}
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