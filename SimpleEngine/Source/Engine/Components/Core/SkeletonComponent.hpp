#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace ECS
{

	struct SkeletonComponent final
	{
		Math::Matrix4x4f jointMatrices[SIMPLE_MAX_BONES];

		Graphics::Animation animation;
		Graphics::AnimationPlayer animationPlayer;

		const Graphics::Skeleton* skeleton;
		const Graphics::Shader* shader;
	};
}