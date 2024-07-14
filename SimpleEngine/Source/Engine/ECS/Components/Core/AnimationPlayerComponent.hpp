#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Animation/AnimationPlayer.hpp"

namespace ECS
{
	struct AnimationPlayerComponent final
	{
		AnimationPlayerComponent();

		Graphics::AnimationPlayer animationPlayer;
		Math::Matrix4x4f jointMatrices[Graphics::Global_Max_Joints];
		Graphics::Animation animation;
		const Graphics::Skeleton* skeleton;
		const Graphics::Shader* shader;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(AnimationPlayerComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationPlayerComponent::animationPlayer);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationPlayerComponent::animation);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationPlayerComponent::skeleton);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationPlayerComponent::shader);
}