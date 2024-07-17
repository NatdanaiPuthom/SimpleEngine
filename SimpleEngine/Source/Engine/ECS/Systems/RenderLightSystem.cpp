#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/SkyBoxComponent.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"
#include "Editor/Editor.hpp"

namespace ECS
{
	RenderLightSystem::RenderLightSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	RenderLightSystem::~RenderLightSystem()
	{
	}

	void RenderLightSystem::Init()
	{
	}

	void RenderLightSystem::Update()
	{
		/*
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		static Math::Vector4f ambientLight = graphicsEngine->GetAmbientLightColorAndIntensity();

		if (ImGui::Begin("Light"))
		{
			if (ImGui::DragFloat4("Ambient light", &ambientLight.x, 0.1f))
			{
				graphicsEngine->SetAmbientLightColorAndIntensity(ambientLight);
			}
		}
		ImGui::End();
		*/
	}

	void RenderLightSystem::Render()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		PointLightData pointLight1;
		pointLight1.color = { 1.0f, 0.0f,0.0f, 5.0f };
		pointLight1.position = { 2.75f, 1.7f, 1.25f };
		pointLight1.radius = 3.0f;
		graphicsEngine->AddPointLight(pointLight1);
	}

	void RenderLightSystem::LateRender()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		ID3D11DeviceContext* context = graphicsEngine->GetContext().Get(); context;
		Drawer::Renderer* renderer = Global::GetRenderer();

		std::vector<Graphics::RenderTarget>& gBuffers = graphicsEngine->GetRenderTargets(Graphics::eRenderTargetType::GBuffer);

		ID3D11ShaderResourceView* shaderResources[5] = {};

		for (size_t i = 0; i < gBuffers.size(); ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		context->PSSetShaderResources(5, 5, shaderResources);

		const Graphics::Mesh* mesh = Global::GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Sphere);
		std::shared_ptr<const Graphics::Shader> shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::PointLight);
		std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTexture(Graphics::eTextureType::Default);
		Math::Transform transform;

		const Graphics::eRasterizerState previousRasterizerState = graphicsEngine->GetCurrentRasterizerState();

		graphicsEngine->SetRasterizerState(Graphics::eRasterizerState::FrontFaceCulling);
		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Greater);
		graphicsEngine->SetBlendState(Graphics::eBlendState::AdditiveBlend);
		graphicsEngine->UpdateLightBuffer();

		PointLightData* pointLightBuffer = graphicsEngine->GetPointLightDataArray();

		for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
		{
			graphicsEngine->UpdatePointlights(i);

			transform.SetPosition(pointLightBuffer[i].position);
			transform.SetScale(pointLightBuffer[i].radius);

			renderer->RenderUnlitStaticModel(transform.GetMatrix(), mesh, shader.get(), texture.get());
		}

		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Less_Equal);
		graphicsEngine->SetRasterizerState(previousRasterizerState);
		graphicsEngine->SetBlendState(Graphics::eBlendState::Disabled);

		ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
		context->PSSetShaderResources(5, 5, nullSRVs);

		if (renderer->GetShouldRenderDebugLines() == true)
		{
			Drawer::Sphere pointLightDebugSpheres;
			pointLightDebugSpheres.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
			{
				pointLightDebugSpheres.position = pointLightBuffer[i].position;
				pointLightDebugSpheres.radius = pointLightBuffer[i].radius;
				renderer->Push(pointLightDebugSpheres);

				pointLightDebugSpheres.radius = 0.1f;
				renderer->Push(pointLightDebugSpheres);
			}
		}
	}

	std::unique_ptr<System> RenderLightSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RenderLightSystem>(aEntityComponentSystem);
	}
}