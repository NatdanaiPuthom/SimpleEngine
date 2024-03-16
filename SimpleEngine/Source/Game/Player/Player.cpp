#include "Game/Precomplied/GamePch.hpp"
#include "Game/Player/Player.hpp"
#include "Game/Player/States/PlayerIdle.hpp"
#include "Game/Player/States/PlayerWalk.hpp"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	myStates[Cast(ePlayerState::Idle)] = std::make_shared<PlayerIdle>(this);
	myStates[Cast(ePlayerState::Walk)] = std::make_shared<PlayerWalk>(this);
	myStates[Cast(ePlayerState::Idle)]->Init();
	myStates[Cast(ePlayerState::Walk)]->Init();
	
	LoadModel();

	SetAnimationState(ePlayerAnimation::Idle, true);

	myCurrentState = myStates[Cast(ePlayerState::Idle)];
}

void Player::Update()
{
	myCurrentState->Update();
	myAnimationPlayer.Update();
}

void Player::Render()
{
	myCurrentState->Render();
	Global::GetRenderer()->RenderModel(myModel);
}

void Player::SetState(const ePlayerState aState)
{
	if (myCurrentState != nullptr)
	{
		myCurrentState->OnStateExit();
	}

	myCurrentState = myStates[Cast(aState)];
	myCurrentState->OnStateEnter();
}

void Player::SetAnimationState(const ePlayerAnimation aAnimation, const bool aShouldLoop)
{
	myAnimationPlayer.Init(*myAnimation[Cast(aAnimation)], myModel);
	myAnimationPlayer.SetIsLooping(aShouldLoop);
	myAnimationPlayer.Play();
}

void Player::LoadModel()
{
	myModel = Global::GetModelFactory()->LoadAnimatedModelFBX("AnimatedModels/SM_Wizard.fbx");
	myModel.SetScale(0.01f);
	myModel.ClearTextures();
	myModel.AddTexture("Models/SM_Wizard_c.dds");

	myAnimation[Cast(ePlayerAnimation::Idle)] = std::make_shared<Simple::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Idle.fbx"));
	myAnimation[Cast(ePlayerAnimation::Walk)] = std::make_shared<Simple::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Walk.fbx"));
}