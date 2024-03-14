#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Engine/Global.hpp"

Simple::AnimationPlayer::AnimationPlayer()
	: myAnimation(nullptr)
	, myModel(nullptr)
	, myTime(0.0f)
	, myFPS(0.0f)
	, myIsLooping(false)
	, myState(eAnimationState::NoAnimation)
{
}

Simple::AnimationPlayer::~AnimationPlayer()
{
}

void Simple::AnimationPlayer::Init(Animation& aAnimation, AnimatedModel& aModel)
{
	myAnimation = &aAnimation;
	myModel = &aModel;
	myFPS = aAnimation.framesPerSecond;
}

void Simple::AnimationPlayer::Play()
{
	myState = eAnimationState::Playing;
}

void Simple::AnimationPlayer::SetIsLooping(const bool aShouldLoop)
{
	myIsLooping = aShouldLoop;
}

void Simple::AnimationPlayer::Update()
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
		const float delta = result - static_cast<float>(frame); //This is use for Lerping & Blend

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

		for (size_t i = 0; i < skeleton->myJoints.size() - 1; i++)
		{
			const Math::Matrix4x4f currentMatrix = myAnimation->frames[frame].localMatrix.find(skeleton->myJoints[i].myName)->second;
			const Math::Matrix4x4f nextMatrix = myAnimation->frames[nextFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;

			Math::Vector3f currentPosition;
			Math::Vector3f nextPosition;

			Math::Quaternionf currentQuaternion;
			Math::Quaternionf nextQuaternion;

			Math::Vector3f currentScale;
			Math::Vector3f nextScale;

			currentMatrix.DecomposeMatrix(currentPosition, currentQuaternion, currentScale);
			nextMatrix.DecomposeMatrix(nextPosition, nextQuaternion, nextScale);

			const Math::Vector3f T = Math::Lerp(currentPosition, nextPosition, delta);
			const Math::Quaternionf R = Math::Quaternionf::Slerp(currentQuaternion, nextQuaternion, delta);
			const Math::Vector3f S = Math::Lerp(currentScale, nextScale, delta);

			const Math::Matrix4x4f Result = Math::Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4() * Math::Matrix4x4f::CreateTranslationMatrix(T);
			myLocalSpacePose.jointTransforms[i] = Result;
		}

		myLocalSpacePose.count = skeleton->myJoints.size();
		myModel->SetPose(myLocalSpacePose);
	}
}

bool Simple::AnimationPlayer::UpdateForThreadedTest(AnimatedModel& aModel, Animation& aAnimation)
{
	if (myState == eAnimationState::Playing)
	{
		myTime += Global::GetDeltaTime();

		if (myTime >= aAnimation.duration)
		{
			if (myIsLooping)
			{
				while (myTime >= aAnimation.duration)
				{
					myTime -= aAnimation.duration;
				}
			}
			else
			{
				myTime = aAnimation.duration;
				myState = eAnimationState::Finished;
			}
		}

		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const size_t frame = static_cast<size_t>(std::floor(result));

		size_t nextFrame = frame + 1;

		if (myState == eAnimationState::Finished)
		{
			nextFrame = frame;
		}
		else if (nextFrame > aAnimation.length)
		{
			nextFrame = 0;
		}

		const Skeleton* skeleton = aModel.GetSkeleton();

		for (size_t i = 0; i < skeleton->myJoints.size() - 1; i++)
		{
			const Math::Matrix4x4f Result = aAnimation.frames[nextFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;
			myLocalSpacePose.jointTransforms[i] = Result;
		}

		myLocalSpacePose.count = skeleton->myJoints.size();
	}

	aModel.SetPose(myLocalSpacePose);

	return true;
}