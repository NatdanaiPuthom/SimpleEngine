#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Engine/Global.hpp"

Simple::AnimationPlayer::AnimationPlayer()
	: myAnimation(nullptr)
	, myModel(nullptr)
	, myTime(0.0f)
	, myFPS(0.0f)
	, myIsLooping(false)
	, myIsInterpolating(true)
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
		//const float delta = result - static_cast<float>(frame); //This is use for Lerping & Blend

		size_t nextFrame = frame + 1;

		if (myState == eAnimationState::Finished)
		{
			nextFrame = frame;
		}
		else if (nextFrame > myAnimation->length)
		{
			nextFrame = 0;
		}

		const Skeleton* skeleton = &myModel->GetSkeleton();

		for (size_t i = 0; i < skeleton->myJoints.size() - 1; i++)
		{
			const Math::Transform& currentFrameJointXform = myAnimation->frames[frame].localTransforms.find(skeleton->myJoints[i].myName)->second;
			Math::Matrix4x4f jointXform = currentFrameJointXform.GetMatrix();

			//I need to switch to Quaternion and have some calcuation correct before I can blend animations

			/*if (myIsInterpolating)
			{
				const Math::Transform& nextFrameJointXform = myAnimation->frames[nextFrame].localTransforms.find(skeleton->myJoints[i].myName)->second;

				const Math::Vector3f T = Math::Lerp(currentFrameJointXform.GetPosition(), nextFrameJointXform.GetPosition(), delta);
				const Math::Vector3f R = Math::Lerp(currentFrameJointXform.GetRotation(), nextFrameJointXform.GetRotation(), delta);
				const Math::Vector3f S = Math::Lerp(currentFrameJointXform.GetScale(), nextFrameJointXform.GetScale(), delta);

				Math::Matrix4x4f rotationMatrix;
				rotationMatrix.SetLocalRotation(R);

				jointXform = Math::Matrix4x4f::CreateScaleMatrix(S) * rotationMatrix * Math::Matrix4x4f::CreateTranslationMatrix(T);
			}*/

			const Math::Matrix4x4f Result = myAnimation->frames[nextFrame].localMatrix.find(skeleton->myJoints[i].myName)->second;
			myLocalSpacePose.jointTransforms[i] = Result;
		}

		myLocalSpacePose.count = skeleton->myJoints.size();
	}
}