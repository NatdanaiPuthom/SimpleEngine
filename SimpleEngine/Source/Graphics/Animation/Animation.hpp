#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace Simple
{
	class Model;
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
		};

		std::vector<Frame> frames;
		std::string name;

		float duration;
		float framesPerSecond;
		
		unsigned int length;
	};

	class AnimationPlayer
	{
	public:
		LocalSpacePose myLocalSpacePose;
	public:
		AnimationPlayer();
		~AnimationPlayer();

		void Init(Animation& aAnimation, Model& aModel);
		void Update();

		void Play();
		void SetIsLooping(const bool aShouldLoop);
	private:
		Animation* myAnimation;
		Model* myModel;

		eAnimationState myState;

		float myTime;
		float myFPS;
		bool myIsLooping;
		bool myIsInterpolating;
	};
}