#include "Game/Precomplied/GamePch.hpp"
#include "Game/Player/States/PlayerIdle.hpp"
#include "Game/Player/Player.hpp"

PlayerIdle::PlayerIdle(Player* aPlayer)
	: PlayerState(aPlayer)
{
}

PlayerIdle::~PlayerIdle()
{
}

void PlayerIdle::OnStateEnter()
{
	myPlayer->SetAnimation(ePlayerAnimation::Idle, true);
}

void PlayerIdle::OnStateExit()
{
}

void PlayerIdle::Init()
{
}

void PlayerIdle::Update()
{
	if (SU::InputManager::GetInstance().IsKeyHeld(VK_UP) ||
		SU::InputManager::GetInstance().IsKeyHeld(VK_DOWN) ||
		SU::InputManager::GetInstance().IsKeyHeld(VK_LEFT) ||
		SU::InputManager::GetInstance().IsKeyHeld(VK_RIGHT))
	{
		myPlayer->SetState(ePlayerState::Walk);
		return;
	}
}