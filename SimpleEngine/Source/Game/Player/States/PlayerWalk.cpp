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
	Math::Vector3 velocity;

	if (SU::InputManager::GetInstance().IsKeyHeld(VK_LEFT))
	{
		velocity.x += -1.0f;
	}

	if (SU::InputManager::GetInstance().IsKeyHeld(VK_RIGHT))
	{
		velocity.x += 1.0f;
	}

	if (SU::InputManager::GetInstance().IsKeyHeld(VK_UP))
	{
		velocity.z += 1.0f;
	}

	if (SU::InputManager::GetInstance().IsKeyHeld(VK_DOWN))
	{
		velocity.z += -1.0f;
	}

	if (velocity == Math::Vector3f::Zero())
	{
		myPlayer->SetState(ePlayerState::Idle);
		return;
	}

	velocity.Normalize();

	Math::Vector3 position = myPlayer->GetPosition();
	position += 2.0f * velocity * Global::GetDeltaTime();

	myPlayer->SetPosition(position);

	auto lookAt = position + 2.0f * velocity;
	myPlayer->LookAt(lookAt);
}

void PlayerWalk::Render()
{
}