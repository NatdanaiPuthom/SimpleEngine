#pragma once

class Player;

 enum class ePlayerState : size_t
{
	Idle,
	Walk,
	Count
};

 enum class ePlayerAnimation : size_t
{
	Idle,
	Walk,
	Count
};

template<typename T>
constexpr inline size_t Cast(const T aValue)
{
	return static_cast<size_t>(aValue);
}

class PlayerState
{
public:
	PlayerState(Player* aPlayer);
	virtual ~PlayerState() = default;

	virtual void OnStateEnter();
	virtual void OnStateExit();

	virtual void Init();
	virtual void Update();
	virtual void Render();
protected:
	Player* myPlayer;
};