#pragma once
#include "Components/Base/Component.h"
#include <Game/GameCommon.h>

class SpriteComponent final : public Component
{
public:
	SpriteComponent() {};
	~SpriteComponent() override {};

	SpriteId mySpriteID;

	constexpr static eComponent myType = eComponent::Sprite;
};