#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"

namespace ECS
{
	MeshComponent::MeshComponent()
		: mesh(nullptr)
		, shader(nullptr)
		, textures{nullptr}
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);

		shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();

		textures[Graphics::Global_Slot_Albedo] = graphicsEngine->GetTexture(Graphics::eTextureType::Default_Albedo).get();
		textures[Graphics::Global_Slot_Normal] = graphicsEngine->GetTexture(Graphics::eTextureType::Default_Normal).get();
		textures[Graphics::Global_Slot_Material] = graphicsEngine->GetTexture(Graphics::eTextureType::Default_Material).get();
	}
}