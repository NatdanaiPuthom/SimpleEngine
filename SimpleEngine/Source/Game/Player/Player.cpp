#include "Game/Precomplied/GamePch.hpp"
#include "Game/Player/Player.hpp"
#include "Game/Player/States/PlayerIdle.hpp"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	myStates[Cast(ePlayerState::Idle)] = std::make_shared<PlayerIdle>();
	myStates[Cast(ePlayerState::Idle)]->Init();
	
	LoadModel();

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

void Player::LoadModel()
{
	myModel = Global::GetModelFactory()->LoadAnimatedModelFBX("AnimatedModels/SM_Wizard.fbx");
	myModel.SetScale(0.01f);
	myModel.ClearTextures();
	myModel.AddTexture("Models/SM_Wizard_c.dds");

	myAnimation = Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Idle.fbx");

	myAnimationPlayer.Init(myAnimation, myModel);
	myAnimationPlayer.SetIsLooping(true);
	myAnimationPlayer.Play();
}