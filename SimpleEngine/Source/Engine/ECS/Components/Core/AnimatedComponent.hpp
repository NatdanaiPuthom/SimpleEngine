#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace ECS
{
	struct AnimatedComponent final
	{
		Math::Matrix4x4f jointMatrices[Graphics::Global_Max_Joints];
		Graphics::Animation animation;
		const Graphics::Skeleton* skeleton = nullptr;
		const Graphics::Shader* shader = nullptr;
	private:
		char padding[48] = "Never give up on your dreams! Just believeeeee\0";
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(AnimatedComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimatedComponent::skeleton, true, false);
	REGISTER_AND_EXPOSE_PROPERTY(&AnimatedComponent::shader, true, false);
}