#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Animation/AnimationPlayer.hpp"

namespace ECS
{
	COMPONENT(AnimationComponent) final
	{
		AnimationComponent();

		Graphics::AnimationPlayer animationPlayer;
		Math::Matrix4x4f jointMatrices[Graphics::Global_Max_Joints];
		const Graphics::Animation* animation;
		const Graphics::Skeleton* skeleton;
		const Graphics::Shader* shader;
	private:
		char padding[8] = "Believ\0";
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationComponent::animation);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimationComponent::skeleton);
}