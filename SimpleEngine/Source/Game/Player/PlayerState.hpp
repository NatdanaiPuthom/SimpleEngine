#pragma once

constexpr enum class ePlayerState : size_t
{
	Idle,
	Walk,
	Count
};

constexpr size_t Cast(const ePlayerState aState)
{
	return static_cast<size_t>(aState);
}

class PlayerState
{
public:
	PlayerState();
	virtual ~PlayerState() = default;

	virtual void OnStateEnter();
	virtual void OnStateExit();

	virtual void Init();
	virtual void Update();
	virtual void Render();
};