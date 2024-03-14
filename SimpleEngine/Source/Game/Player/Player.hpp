#pragma once
#include "Game/Player/PlayerState.hpp"

class Player final
{
public:
	Player();
	~Player();

	void Init();
	void Update();
	void Render();
private:
	void LoadModel();

	std::shared_ptr<PlayerState> myCurrentState;

	Simple::Animation myAnimation;
	Simple::AnimatedModel myModel;
	Simple::AnimationPlayer myAnimationPlayer;

	std::array<std::shared_ptr<PlayerState>, static_cast<size_t>(ePlayerState::Count)> myStates;
};