#include "Game/Precomplied/GamePch.hpp"
//#include "Game/Player/Player.hpp"
//#include "Game/Player/PlayerState.hpp"
//#include "Game/Player/States/PlayerIdle.hpp"
//#include "Game/Player/States/PlayerWalk.hpp"
//#include "Game/AnimationController/AnimationController.hpp"
//
//Player::Player()
//{
//}
//
//Player::~Player()
//{
//}
//
//void Player::Init()
//{
//	myAnimationController = std::make_unique<Simple::AnimationController>();
//
//	myStates[Cast(ePlayerState::Idle)] = std::make_shared<PlayerIdle>(this);
//	myStates[Cast(ePlayerState::Walk)] = std::make_shared<PlayerWalk>(this);
//
//	LoadModel();
//
//	myStates[Cast(ePlayerState::Idle)]->Init();
//	myStates[Cast(ePlayerState::Walk)]->Init();
//
//	//myAnimationController->Init(myAnimatedModel.get(), myAnimations[Cast(ePlayerAnimation::Idle)].get(), true);
//
//	myCurrentState = myStates[Cast(ePlayerState::Idle)];
//
//	myAnimationController->Update();
//}
//
//void Player::Update()
//{
//	myCurrentState->Update();
//}
//
//void Player::Render()
//{
//	myCurrentState->Render();
//}
//
//void Player::SetState(const ePlayerState aState)
//{
//	if (myCurrentState != nullptr)
//	{
//		myCurrentState->OnStateExit();
//	}
//
//	myCurrentState = myStates[Cast(aState)];
//	myCurrentState->OnStateEnter();
//}
//
//void Player::SetAnimation(const ePlayerAnimation aAnimation, const bool aShouldLoop, const float aDuration)
//{
//	myAnimationController->ChangeAnimation(myAnimations[Cast(aAnimation)].get(), aShouldLoop, aDuration);
//}
//
//void Player::SetVelocity(const Math::Vector3f& aVelocity)
//{
//	myVelocity = aVelocity;
//}
//
//Math::Vector3f Player::GetVelocity() const
//{
//	return myVelocity;
//}
//
//void Player::LoadModel()
//{
//	myAnimations[Cast(ePlayerAnimation::Idle)] = std::make_shared<Graphics::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Idle.fbx"));
//	myAnimations[Cast(ePlayerAnimation::Walk)] = std::make_shared<Graphics::Animation>(Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Walk.fbx"));
//}