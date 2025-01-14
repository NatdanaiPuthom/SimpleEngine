#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"
#include "Engine/Global.hpp"

namespace ECS
{
	DirectionalLightComponent::DirectionalLightComponent()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
		texture = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::DirectionalLight).get();
		shader = graphicsEngine->GetShaderManager()->GetShader(Graphics::eShaderType::Unlit_Default).get();
	}
}