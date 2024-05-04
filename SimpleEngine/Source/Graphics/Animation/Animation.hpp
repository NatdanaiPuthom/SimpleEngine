#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace Graphics
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

namespace Graphics
{
	struct Animation
	{
		struct Frame
		{
			std::unordered_map<std::string, Math::Matrix4x4f> jointNameToModelSpaceMatrix;
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
		ModelSpacePose myModelSpacePose;

	public:
		void Init(Animation& aAnimation, const Skeleton* aSkeleton);
		void UpdateTest(Math::Matrix4x4f* aMatrix);
		void LerpAnimationTest(const size_t aCurrentFrame, const size_t aNextFrame, const float aDelta);

		AnimationPlayer();
		~AnimationPlayer();

		void Init(Animation& aAnimation, AnimatedModel& aModel);

		//Will lerp active animation and setpose directly
		void Update();

		//Update and SetPose of multiple models and animation of same kind
		void UpdateMultipleModels(std::vector<AnimatedModel*>& aModelContainer);

		//Lerp active animation and store the pose
		void LerpCurrentAnimation();

		void Play(const bool aShouldLoop = true); //NOTE(v9.34.0): Default to true because my smol brain keep forgetting and wondering why animation doesn't play everytime i rewrote system to test
		void Pause();
		void Stop();
		void Restart();

		void SetIsLooping(const bool aShouldLoop);
		void SetCurrentFrame(const unsigned int aCurrentFrame);

		const ModelSpacePose GetLocalSpacePose() const;
		eAnimationState GetAnimationState() const;
		float GetTime() const;
		unsigned int GetCurrentFrame() const;

	private:
		void InitPose();
		void CalculateFrame(size_t& aCurrentFrame, size_t& aNextFrame, float& aDelta);
		void LerpAnimation(const size_t aCurrentFrame, const size_t aNextFrame, const float aDelta);
		void UpdateTimer();

	private:
		eAnimationState myState;

		Animation* myAnimation;
		AnimatedModel* myModel;

		const Skeleton* mySkeleton;


		float myTime;
		float myFPS;
		bool myIsLooping;
	};
}