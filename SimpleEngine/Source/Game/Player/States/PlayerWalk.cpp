#include "Game/Precomplied/GamePch.hpp"
#include "Game/Player/States/PlayerWalk.hpp"
#include "Game/Player/Player.hpp"

PlayerWalk::PlayerWalk(Player* aPlayer)
	: PlayerState(aPlayer)
{
}

PlayerWalk::~PlayerWalk()
{
}

void PlayerWalk::OnStateEnter()
{
	myPlayer->SetAnimation(ePlayerAnimation::Walk, true);
}

void PlayerWalk::OnStateExit()
{
}

void PlayerWalk::Init()
{
}

void PlayerWalk::Update()
{
	if (SU::InputManager::GetInstance().IsKeyPressed(VK_LEFT))
	{
		myPlayer->SetState(ePlayerState::Idle);
	}
}

void PlayerWalk::Render()
{
}