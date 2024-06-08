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
	/*if (MainSingleton::GetInputManager().IsKeyHeld(VK_UP) ||
		MainSingleton::GetInputManager().IsKeyHeld(VK_DOWN) ||
		MainSingleton::GetInputManager().IsKeyHeld(VK_LEFT) ||
		MainSingleton::GetInputManager().IsKeyHeld(VK_RIGHT))
	{
		myPlayer->SetState(ePlayerState::Walk);
		return;
	}*/
}