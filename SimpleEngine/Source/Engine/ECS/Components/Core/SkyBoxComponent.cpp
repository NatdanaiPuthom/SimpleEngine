#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Global.hpp"

namespace ECS
{
	SkyBoxComponent::SkyBoxComponent()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::SkyBox);
		texture = graphicsEngine->GetTextureManager()->GetSkyBox(Graphics::eSkyBox::AutumnForest).get();
		shader = graphicsEngine->GetShaderManager()->GetShader(Graphics::eShaderType::SkyBox).get();
	}
}