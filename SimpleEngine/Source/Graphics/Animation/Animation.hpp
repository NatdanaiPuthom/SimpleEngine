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
	NotStarted,
	Playing,
	Stopped,
	Paused,
	Finished
};

namespace Simple
{
	struct Animation
	{
		struct Frame
		{
			std::unordered_map<std::string, Math::Transform> localTransforms;
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
		void Update();
		bool UpdateThreaded(AnimatedModel& aModel, Animation& aAnimation);

		void Play();
		void SetIsLooping(const bool aShouldLoop);
	private:
		Animation* myAnimation;
		AnimatedModel* myModel;

		eAnimationState myState;

		float myTime;
		float myFPS;
		bool myIsLooping;
		bool myIsInterpolating;
	};
}