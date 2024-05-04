#pragma once
#include <Game/GameUpdateContext.h>

enum class eComponent
{
	Player,
	Sprite,
	Script,
};

static constexpr const char* componentNames = "Player\0Sprite\0Script";

class GameObject;

class Component
{
public:
	Component() {};
	virtual ~Component() = default;

	virtual void Init(const GameInitContext& aContext) { aContext; };
	virtual void Update(const GameUpdateContext& aContext) { aContext; };

	GameObject* myGameObject = nullptr;
};