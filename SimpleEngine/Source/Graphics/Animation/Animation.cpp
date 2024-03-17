#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	AnimationPlayer::AnimationPlayer()
		: myAnimation(nullptr)
		, myModel(nullptr)
		, myTime(0.0f)
		, myFPS(0.0f)
		, myIsLooping(false)
		, myState(eAnimationState::NoAnimation)
	{
	}

	AnimationPlayer::~AnimationPlayer()
	{
	}

	void AnimationPlayer::Init(Animation& aAnimation, AnimatedModel& aModel)
	{
		myAnimation = &aAnimation;
		myModel = &aModel;
		myFPS = aAnimation.framesPerSecond;
		myTime = 0.0f;
	}

	void AnimationPlayer::Play()
	{
		myState = eAnimationState::Playing;
	}

	void AnimationPlayer::Pause()
	{
		myState = eAnimationState::Paused;
	}

	void AnimationPlayer::Stop()
	{
		myState = eAnimationState::Finished;
		myTime = 0.0f;
	}

	void AnimationPlayer::Restart()
	{
		myTime = 0.0f;
		myState = eAnimationState::Playing;
	}

	void AnimationPlayer::Update()
	{
		if (myState == eAnimationState::Playing)
		{
			LerpCurrentAnimation();
			myModel->SetPose(myLocalSpacePose);
		}
	}

	void AnimationPlayer::UpdateMultipleModels(std::vector<Simple::AnimatedModel*>& aModelContainer)
	{
		if (myState == eAnimationState::Playing)
		{
			LerpCurrentAnimation();

			ModelSpacePose modelSpacePose;
			aModelContainer[0]->GetSkeleton()->ConvertPoseToModelSpace(myLocalSpacePose, modelSpacePose);

			for (auto& model : aModelContainer)
			{
				model->SetPose(modelSpacePose);
			}
		}
	}

	void AnimationPlayer::LerpCurrentAnimation()
	{
		if (myState == eAnimationState::Playing)
		{
			myTime += Global::GetDeltaTime();

			if (myTime >= myAnimation->duration)
			{
				if (myIsLooping)
				{
					while (myTime >= myAnimation->duration)
					{
						myTime -= myAnimation->duration;
					}
				}
				else
				{
					myTime = myAnimation->duration;
					myState = eAnimationState::Finished;
				}
			}

			const float frameRate = 1.0f / myFPS;
			const float result = myTime / frameRate;
			const size_t frame = static_cast<size_t>(std::floor(result));
			const float delta = result - static_cast<float>(frame); //This is use for Lerping

			size_t nextFrame = frame + 1;

			if (myState == eAnimationState::Finished)
			{
				nextFrame = frame;
			}
			else if (nextFrame > myAnimation->length)
			{
				nextFrame = 0;
			}

			const Skeleton* skeleton = myModel->GetSkeleton();

			Math::Vector3f currentPosition;
			Math::Vector3f nextPosition;

			Math::Quaternionf currentQuaternion;
			Math::Quaternionf nextQuaternion;

			Math::Vector3f currentScale;
			Math::Vector3f nextScale;

			for (size_t i = 0; i < skeleton->myJoints.size() - 1; i++)
			{
				const Math::Matrix4x4f currentMatrix = myAnimation->frames[frame].localMatrix.find(skeleton->myJoints[i].myName)->second;
				const Math::Matrix4x4f nextMatrix = myAnimation->frames[nextFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;

				currentMatrix.DecomposeMatrix(currentPosition, currentQuaternion, currentScale);
				nextMatrix.DecomposeMatrix(nextPosition, nextQuaternion, nextScale);

				const Math::Vector3f translation = Math::Lerp(currentPosition, nextPosition, delta);
				const Math::Quaternionf rotation = Math::Quaternionf::Slerp(currentQuaternion, nextQuaternion, delta);
				const Math::Vector3f scale = Math::Lerp(currentScale, nextScale, delta);

				const Math::Matrix4x4f lerpedMatrix = Math::Matrix4x4f::CreateScaleMatrix(scale) * rotation.GetRotationMatrix4x4() * Math::Matrix4x4f::CreateTranslationMatrix(translation);
				myLocalSpacePose.jointTransforms[i] = lerpedMatrix;
			}

			myLocalSpacePose.count = skeleton->myJoints.size();
		}
	}

	void AnimationPlayer::SetIsLooping(const bool aShouldLoop)
	{
		myIsLooping = aShouldLoop;
	}

	eAnimationState AnimationPlayer::GetAnimationState() const
	{
		return myState;
	}
}