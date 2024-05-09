#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Animation/AnimationPlayer.hpp"
#include "Engine/Global.hpp"

#include "Engine/ECS/Components/Core/AnimatedComponent.hpp"

namespace Graphics
{
	AnimationPlayer::AnimationPlayer()
		: myAnimation(nullptr)
		, myTime(0.0f)
		, myFPS(0.0f)
		, myIsLooping(false)
		, myState(eAnimationState::NoAnimation)
	{
	}

	AnimationPlayer::~AnimationPlayer()
	{
	}

	void AnimationPlayer::Init(Animation& aAnimation, const Skeleton* aSkeleton)
	{
		myAnimation = &aAnimation;
		mySkeleton = aSkeleton;

		myFPS = aAnimation.framesPerSecond;
		myTime = 0.0f;

		myState = eAnimationState::Playing;
	}

	void AnimationPlayer::UpdateTest(Math::Matrix4x4f* aMatrix, ECS::AnimatedComponent* aAnimatedComponent)
	{
		if (myState == eAnimationState::Playing)
		{
			myTime += Global::GetDeltaTime();

			if (myTime >= aAnimatedComponent->animation.duration)
			{
				if (myIsLooping)
				{
					while (myTime >= aAnimatedComponent->animation.duration)
					{
						myTime -= aAnimatedComponent->animation.duration;
					}
				}
				else
				{
					myTime = aAnimatedComponent->animation.duration;
					myState = eAnimationState::Finished;
				}
			}

			const float frameRate = 1.0f / aAnimatedComponent->animation.framesPerSecond;
			const float result = myTime / frameRate;
			const size_t currentFrame = static_cast<size_t>(std::floor(result));
			const float delta = result - static_cast<float>(currentFrame);

			size_t nextFrame = currentFrame + 1;

			if (myState == eAnimationState::Finished)
			{
				nextFrame = currentFrame;
			}
			else if (nextFrame > aAnimatedComponent->animation.length)
			{
				nextFrame = 0;
			}

			const Skeleton* skeleton = aAnimatedComponent->skeleton;
			myModelSpacePose.count = skeleton->myJoints.size();

			for (size_t i = 0; i < myModelSpacePose.count; i++)
			{
				const Math::Matrix4x4f currentMatrix = aAnimatedComponent->animation.frames[currentFrame].jointNameToModelSpaceMatrix.find(skeleton->myJoints[i].myName)->second;
				const Math::Matrix4x4f nextMatrix = aAnimatedComponent->animation.frames[nextFrame].jointNameToModelSpaceMatrix.find(skeleton->myJoints[i].myName)->second;

				Math::Vector3f currentPosition;
				Math::Vector3f nextPosition;

				Math::Quaternionf currentQuaternion;
				Math::Quaternionf nextQuaternion;

				Math::Vector3f currentScale;
				Math::Vector3f nextScale;

				currentMatrix.DecomposeMatrix(currentPosition, currentQuaternion, currentScale);
				nextMatrix.DecomposeMatrix(nextPosition, nextQuaternion, nextScale);

				const Math::Vector3f translation = Math::Lerp(currentPosition, nextPosition, delta);
				const Math::Quaternionf rotation = Math::Quaternionf::Slerp(currentQuaternion, nextQuaternion, delta);
				const Math::Vector3f scale = Math::Lerp(currentScale, nextScale, delta);

				const Math::Matrix4x4f lerpedMatrix = Math::Matrix4x4f::CreateScaleMatrix(scale) * rotation.GetRotationMatrix4x4() * Math::Matrix4x4f::CreateTranslationMatrix(translation);
				myModelSpacePose.jointTransforms[i] = lerpedMatrix;
			}

			LocalSpacePose localSpacePose;
			skeleton->ConvertModelSpacePoseToLocalSpacePose(myModelSpacePose, localSpacePose);
			skeleton->ApplyBindPoseInverse(localSpacePose, aMatrix);
		}
	}

	void AnimationPlayer::LerpAnimationTest(const size_t aCurrentFrame, const size_t aNextFrame, const float aDelta)
	{
		const Skeleton* skeleton = mySkeleton;
		myModelSpacePose.count = skeleton->myJoints.size();

		for (size_t i = 0; i < myModelSpacePose.count; i++)
		{
			const Math::Matrix4x4f currentMatrix = myAnimation->frames[aCurrentFrame].jointNameToModelSpaceMatrix.find(skeleton->myJoints[i].myName)->second;
			const Math::Matrix4x4f nextMatrix = myAnimation->frames[aNextFrame].jointNameToModelSpaceMatrix.find(skeleton->myJoints[i].myName)->second;

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
			myModelSpacePose.jointTransforms[i] = lerpedMatrix;
		}
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
		return myModelSpacePose;
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

	void AnimationPlayer::CalculateFrame(size_t& aCurrentFrame, size_t& aNextFrame, float& aDelta)
	{
		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const size_t currentFrame = static_cast<size_t>(std::floor(result));
		const float delta = result - static_cast<float>(currentFrame);

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