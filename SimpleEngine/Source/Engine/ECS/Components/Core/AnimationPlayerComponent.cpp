#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Components/Core/AnimationPlayerComponent.hpp"
#include "Engine/Global.hpp"

namespace ECS
{
	AnimationPlayerComponent::AnimationPlayerComponent()
		: skeleton(nullptr)
		, shader(nullptr)
	{
		shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Animated).get();
	}
}