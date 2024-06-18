#pragma once

class PlayerWalk : public PlayerState
{
public:
	PlayerWalk(Player* aPlayer);
	~PlayerWalk() override;

	void OnStateEnter() override;
	void OnStateExit() override;

	void Init() override;
	void Update() override;
	void Render() override;
};