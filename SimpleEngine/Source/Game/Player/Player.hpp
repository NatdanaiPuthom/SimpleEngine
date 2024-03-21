#pragma once
#include "Game/Test/IK/InverseKinematics.hpp"

class PlayerState;

namespace Simple
{
	class AnimationController;
}

class Player final
{
public:
	Player();
	~Player();

	void Init();
	void Update();
	void Render();

public:
	void SetState(const ePlayerState aState);
	void SetAnimation(const ePlayerAnimation aAnimation, const bool aShouldLoop = false, const float aDuration = -1.0f);
	void SetVelocity(const Math::Vector3f& aVelocity);
	void SetPosition(const Math::Vector3f& aPosition);
	void SetRotation(const Math::Vector3f& aRotation);
	void LookAt(const Math::Vector3f& aTargetPoint);
public:
	Math::Vector3f GetVelocity() const;
	Math::Vector3f GetPosition() const;
	Math::Vector3f GetRotation() const;
private:
	void LoadModel();
private:
	Math::Vector3f myVelocity;

	std::shared_ptr<PlayerState> myCurrentState;
	std::shared_ptr<Simple::AnimatedModel> myAnimatedModel;
	std::unique_ptr<Simple::AnimationController> myAnimationController;

	std::array<std::shared_ptr<Simple::Animation>, static_cast<size_t>(ePlayerAnimation::Count)> myAnimations;
	std::array<std::shared_ptr<PlayerState>, static_cast<size_t>(ePlayerState::Count)> myStates;






	Test::InverseKinematics myInverseKinematics;
};