#pragma once

class PlayerState;

namespace Simple
{
	class AnimationController;
}

class Player final
{
public:
	Player();
	~Player();

	void Init();
	void Update();
	void Render();

	void SetState(const ePlayerState aState);
	void SetAnimation(const ePlayerAnimation aAnimation, const bool aShouldLoop = false, const float aDuration = -1.0f);
private:
	void LoadModel();
private:

	std::shared_ptr<PlayerState> myCurrentState;
	std::shared_ptr<Simple::AnimatedModel> myAnimatedModel;
	std::unique_ptr<Simple::AnimationController> myAnimationController;

	std::array<std::shared_ptr<Simple::Animation>, static_cast<size_t>(ePlayerAnimation::Count)> myAnimations;
	std::array<std::shared_ptr<PlayerState>, static_cast<size_t>(ePlayerState::Count)> myStates;
};