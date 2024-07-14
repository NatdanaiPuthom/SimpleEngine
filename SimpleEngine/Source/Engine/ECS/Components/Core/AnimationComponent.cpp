#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Components/Core/AnimationComponent.hpp"
#include "Engine/Global.hpp"

namespace ECS
{
	AnimationComponent::AnimationComponent()
		: skeleton(nullptr)
		, shader(nullptr)
		, animation(nullptr)
	{
		shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Animated).get();
		animationPlayer.Init(animation, skeleton);
		animationPlayer.Play(true);
	}
}