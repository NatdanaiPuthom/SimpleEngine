#pragma once
#include "Game/Player/PlayerState.hpp"

class PlayerState;

class Player final
{
public:
	Player();
	~Player();

	void Init();
	void Update();
	void Render();

	void SetState(const ePlayerState aState);
	void SetAnimationState(const ePlayerAnimation aAnimation, const bool aShouldLoop = false);
private:
	void LoadModel();

	std::shared_ptr<PlayerState> myCurrentState;

	Simple::AnimatedModel myModel;
	Simple::AnimationPlayer myAnimationPlayer;

	std::array<std::shared_ptr<Simple::Animation>, static_cast<size_t>(ePlayerAnimation::Count)> myAnimation;
	std::array<std::shared_ptr<PlayerState>, static_cast<size_t>(ePlayerState::Count)> myStates;
};