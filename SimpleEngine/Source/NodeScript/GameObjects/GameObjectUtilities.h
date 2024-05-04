#pragma once
#include "Base/GameObject.h"
#include "Components/PlayerComponent.h"
#include "Components/SpriteComponent.h"

bool HasComponentByID(const GameObject& aGameObject, const eComponent aComponentID)
{
	switch (aComponentID)
	{
	case eComponent::Player:
		return aGameObject.HasComponent<PlayerComponent>();
		break;
	case eComponent::Sprite:
		return aGameObject.HasComponent<SpriteComponent>();
		break;
	default:
		return false;
		break;
	}
}