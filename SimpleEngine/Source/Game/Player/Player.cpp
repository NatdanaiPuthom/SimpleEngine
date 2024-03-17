#include "Game/Precomplied/GamePch.hpp"
#include "Game/Player/Player.hpp"
#include "Game/Player/PlayerState.hpp"
#include "Game/Player/States/PlayerIdle.hpp"
#include "Game/Player/States/PlayerWalk.hpp"
#include "Game/Test/AnimationController/AnimationController.hpp"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	myAnimationController = std::make_unique<Simple::AnimationController>();

	myStates[Cast(ePlayerState::Idle)] = std::make_shared<PlayerIdle>(this);
	myStates[Cast(ePlayerState::Walk)] = std::make_shared<PlayerWalk>(this);

	LoadModel();

	myStates[Cast(ePlayerState::Idle)]->Init();
	myStates[Cast(ePlayerState::Walk)]->Init();

	myAnimationController->Init(myAnimatedModel.get(), myAnimations[Cast(ePlayerAnimation::Idle)].get(), true);

	myCurrentState = myStates[Cast(ePlayerState::Idle)];
}

void Player::Update()
{
	myCurrentState->Update();
	myAnimationController->Update();
}

void Player::Render()
{
	myCurrentState->Render();

	auto renderer = Global::GetRenderer();
	renderer->RenderModel(myAnimatedModel);
	renderer->RenderBoundingBox(myAnimatedModel);
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

void Player::SetAnimation(const ePlayerAnimation aAnimation, const bool aShouldLoop, const float aDuration)
{
	myAnimationController->ChangeAnimation(myAnimations[Cast(aAnimation)].get(), aShouldLoop, aDuration);
}

void Player::SetVelocity(const Math::Vector3f& aVelocity)
{
	myVelocity = aVelocity;
}

void Player::SetPosition(const Math::Vector3f& aPosition)
{
	myAnimatedModel->SetPosition(aPosition);
}

void Player::SetRotation(const Math::Vector3f& aRotation)
{
	myAnimatedModel->SetRotation(aRotation);
}

void Player::LookAt(const Math::Vector3f& aTargetPoint)
{
	myAnimatedModel->LookAt(aTargetPoint);
}

Math::Vector3f Player::GetVelocity() const
{
	return myVelocity;
}

Math::Vector3f Player::GetPosition() const
{
	return myAnimatedModel->GetPosition();
}

Math::Vector3f Player::GetRotation() const
{
	return myAnimatedModel->GetRotation();
}

void Player::LoadModel()
{
	myAnimatedModel = std::make_shared<Simple::AnimatedModel>(Global::GetModelFactory()->LoadAnimatedModelFBX("AnimatedModels/SM_Wizard.fbx"));
	myAnimatedModel->SetScale(0.01f);
	myAnimatedModel->ClearTextures();
	myAnimatedModel->AddTexture("Models/SM_Wizard_c.dds");

	myAnimations[Cast(ePlayerAnimation::Idle)] = std::make_shared<Simple::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Idle.fbx"));
	myAnimations[Cast(ePlayerAnimation::Walk)] = std::make_shared<Simple::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Walk.fbx"));
}