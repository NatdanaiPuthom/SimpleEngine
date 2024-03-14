#pragma once
#include "Game/Player/PlayerState.hpp"

class PlayerIdle : public PlayerState
{
public:
	PlayerIdle();
	~PlayerIdle() override;

	void OnStateEnter() override;
	void OnStateExit() override;

	void Init() override;
	void Update() override;
};