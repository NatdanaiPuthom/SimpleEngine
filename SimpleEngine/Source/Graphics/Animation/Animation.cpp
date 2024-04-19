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

		InitPose();
	}

	void AnimationPlayer::Play(const bool aShouldLoop)
	{
		myState = eAnimationState::Playing;
		SetIsLooping(aShouldLoop);
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

			LocalSpacePose modelSpacePose;
			aModelContainer[0]->GetSkeleton()->ConvertModelSpacePoseToLocalSpacePose(myLocalSpacePose, modelSpacePose);

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
			size_t currentFrame = 0;
			size_t nextFrame = 0;
			float delta = 0.0f;

			UpdateTimer();
			CalculateFrame(currentFrame, nextFrame, delta);
			LerpAnimation(currentFrame, nextFrame, delta);
		}
	}

	void AnimationPlayer::SetIsLooping(const bool aShouldLoop)
	{
		myIsLooping = aShouldLoop;
	}

	void AnimationPlayer::SetCurrentFrame(const unsigned int aCurrentFrame)
	{
		myTime = aCurrentFrame / myFPS;
	}

	const ModelSpacePose AnimationPlayer::GetLocalSpacePose() const
	{
		return myLocalSpacePose;
	}

	eAnimationState AnimationPlayer::GetAnimationState() const
	{
		return myState;
	}

	float AnimationPlayer::GetTime() const
	{
		return myTime;
	}

	unsigned int AnimationPlayer::GetCurrentFrame() const
	{
		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const unsigned int currentFrame = static_cast<unsigned int>(std::floor(result));

		return currentFrame;
	}

	void AnimationPlayer::InitPose()
	{
		myState = eAnimationState::Playing;

		LerpCurrentAnimation();
		myModel->SetPose(myLocalSpacePose);

		myState = eAnimationState::Finished;
	}

	void AnimationPlayer::CalculateFrame(size_t& aCurrentFrame, size_t& aNextFrame, float& aDelta)
	{
		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const size_t currentFrame = static_cast<size_t>(std::floor(result));
		const float delta = result - static_cast<float>(currentFrame); //This is use for Lerping

		size_t nextFrame = currentFrame + 1;

		if (myState == eAnimationState::Finished)
		{
			nextFrame = currentFrame;
		}
		else if (nextFrame > myAnimation->length)
		{
			nextFrame = 0;
		}

		aCurrentFrame = currentFrame;
		aNextFrame = nextFrame;
		aDelta = delta;
	}

	void AnimationPlayer::LerpAnimation(const size_t aCurrentFrame, const size_t aNextFrame, const float aDelta)
	{
		const Skeleton* skeleton = myModel->GetSkeleton();

		for (size_t i = 0; i < skeleton->myJoints.size(); i++)
		{
			const Math::Matrix4x4f currentMatrix = myAnimation->frames[aCurrentFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;
			const Math::Matrix4x4f nextMatrix = myAnimation->frames[aNextFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;

			Math::Vector3f currentPosition;
			Math::Vector3f nextPosition;

			Math::Quaternionf currentQuaternion;
			Math::Quaternionf nextQuaternion;

			Math::Vector3f currentScale;
			Math::Vector3f nextScale;

			currentMatrix.DecomposeMatrix(currentPosition, currentQuaternion, currentScale);
			nextMatrix.DecomposeMatrix(nextPosition, nextQuaternion, nextScale);

			const Math::Vector3f translation = Math::Lerp(currentPosition, nextPosition, aDelta);
			const Math::Quaternionf rotation = Math::Quaternionf::Slerp(currentQuaternion, nextQuaternion, aDelta);
			const Math::Vector3f scale = Math::Lerp(currentScale, nextScale, aDelta);

			const Math::Matrix4x4f lerpedMatrix = Math::Matrix4x4f::CreateScaleMatrix(scale) * rotation.GetRotationMatrix4x4() * Math::Matrix4x4f::CreateTranslationMatrix(translation);
			myLocalSpacePose.jointTransforms[i] = lerpedMatrix;
		}

		myLocalSpacePose.count = skeleton->myJoints.size();
	}

	void AnimationPlayer::UpdateTimer()
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
	}
}