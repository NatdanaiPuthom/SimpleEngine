#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace Simple
{
	class AnimatedModel;
}

enum class eAnimationState
{
	NoAnimation,
	Playing,
	Paused,
	Finished
};

namespace Simple
{
	struct Animation
	{
		struct Frame
		{
			std::unordered_map<std::string, Math::Matrix4x4f> localMatrix;
		};

		std::vector<Frame> frames;
		std::string name;

		float duration = 0.0f;
		float framesPerSecond = 0.0f;
		
		unsigned int length = 0;
	};

	class AnimationPlayer
	{
	public:
		LocalSpacePose myLocalSpacePose;
	public:
		AnimationPlayer();
		~AnimationPlayer();

		void Init(Animation& aAnimation, AnimatedModel& aModel);

		//Will lerp active animation and setpose directly
		void Update();

		//Update and SetPose of multiple models and animation of same kind
		void UpdateMultipleModels(std::vector<Simple::AnimatedModel*>& aModelContainer);

		//Lerp active animation and store the pose
		void LerpCurrentAnimation();

		void Play();
		void Pause();
		void Stop();
		void Restart();

		void SetIsLooping(const bool aShouldLoop);
		void SetCurrentFrame(const unsigned int aCurrentFrame);

		eAnimationState GetAnimationState() const;
		float GetTime() const;
		unsigned int GetCurrentFrame() const;
	private:
		Animation* myAnimation;
		AnimatedModel* myModel;

		eAnimationState myState;

		float myTime;
		float myFPS;
		bool myIsLooping;
	};
}