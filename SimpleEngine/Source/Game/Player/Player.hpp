#pragma once

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
public:
	Math::Vector3f GetVelocity() const;
private:
	void LoadModel();
private:
	Math::Vector3f myVelocity;

	std::shared_ptr<PlayerState> myCurrentState;
	std::unique_ptr<Simple::AnimationController> myAnimationController;

	std::array<std::shared_ptr<Graphics::Animation>, static_cast<size_t>(ePlayerAnimation::Count)> myAnimations;
	std::array<std::shared_ptr<PlayerState>, static_cast<size_t>(ePlayerState::Count)> myStates;
};