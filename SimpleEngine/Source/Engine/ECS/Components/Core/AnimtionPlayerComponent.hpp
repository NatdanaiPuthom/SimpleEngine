#pragma once
#include "Graphics/Animation/AnimationPlayer.hpp"

namespace ECS
{
	struct AnimationPlayerComponent final
	{
		Graphics::AnimationPlayer animationPlayer;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(AnimationPlayerComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationPlayerComponent::animationPlayer);
}