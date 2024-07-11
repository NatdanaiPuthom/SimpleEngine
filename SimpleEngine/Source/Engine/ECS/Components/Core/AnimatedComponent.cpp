#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Components/Core/AnimatedComponent.hpp"

namespace ECS
{
	AnimatedComponent::AnimatedComponent()
		: skeleton(nullptr)
		, shader(nullptr)
	{
		shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Animated).get();
	}
}