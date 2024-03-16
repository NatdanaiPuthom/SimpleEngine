#pragma once

class AnimationController final
{
public:
	AnimationController();
	~AnimationController();
private:
	
	Simple::AnimationPlayer myAnimationPlayer;
	Simple::Animation* myCurrentAnimation;
	Simple::Animation* myTargetAnimation;

	float myTimer;
	float myDuration;
	bool myIsInterpolating;
};