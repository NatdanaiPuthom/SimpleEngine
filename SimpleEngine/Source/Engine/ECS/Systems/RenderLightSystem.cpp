#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/SkyBoxComponent.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"
#include "Engine/ECS/Components/Core/PointLightComponent.hpp"
#include "Editor/Editor.hpp"
#include "Graphics/BufferData.hpp"

namespace ECS
{
	RenderLightSystem::RenderLightSystem()
	{
	}

	RenderLightSystem::~RenderLightSystem()
	{
	}

	void RenderLightSystem::Render(EntityComponentSystem* aEntityComponentSystem)
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		const std::unordered_set<EntityID>& entityIDsWithPointLight = aEntityComponentSystem->GetEntityIDsWithThisComponent<ECS::PointLightComponent>();

		for (const EntityID& id : entityIDsWithPointLight)
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(id);
			ECS::TransformComponent* transformComponent = entity.GetComponent<ECS::TransformComponent>();

			if (transformComponent == nullptr)
			{
				continue;
			}

			ECS::PointLightComponent* pointLightComponent = entity.GetComponent<ECS::PointLightComponent>();
			Graphics::PointLightData pointLight;
			pointLight.color = pointLightComponent->pointLightData.color;
			pointLight.position = transformComponent->transform.GetPosition();
			pointLight.radius = pointLightComponent->pointLightData.radius;

			graphicsEngine->GetLightManager()->AddPointLight(pointLight);
		}
	}

	void RenderLightSystem::LateRender(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = graphicsEngine->GetContext();
		Drawer::Renderer* renderer = Global::GetRenderer();

		std::vector<Graphics::RenderTarget>& gBuffers = graphicsEngine->GetRenderTargetManager()->GetRenderTargets(Graphics::eRenderTargetType::GBuffer);

		ID3D11ShaderResourceView* shaderResources[5] = {};

		for (size_t i = 0; i < gBuffers.size(); ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		context->PSSetShaderResources(5, 5, shaderResources);

		const Graphics::Mesh* mesh = Global::GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Sphere);
		std::shared_ptr<const Graphics::Shader> shader = Global::GetGraphicsEngine()->GetShaderManager()->GetShader(Graphics::eShaderType::PointLight);
		std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTextureManager()->GetTexture(Graphics::eTextureType::Default);
		Math::Transform transform;

		Graphics::StateManager* const stateManager = graphicsEngine->GetStateManager();
		const Graphics::eRasterizerState previousRasterizerState = stateManager->GetCurrentRasterizerState();

		stateManager->SetRasterizerState(context, Graphics::eRasterizerState::FrontFaceCulling);
		stateManager->SetDepthStencilState(context, Graphics::eDepthStencilState::Greater);
		stateManager->SetBlendState(context, Graphics::eBlendState::AdditiveBlend);

		Graphics::LightManager* const lightManager = graphicsEngine->GetLightManager();
		Graphics::ConstantBufferManager* const constantBufferManager = graphicsEngine->GetConstantBufferManager();

		constantBufferManager->UpdateLightConstantBuffer(lightManager->GetLightBufferData());

		Graphics::PointLightData* pointLightBuffer = lightManager->GetPointLightDataArray();

		for (size_t i = 0; i < lightManager->GetPointLightCount(); ++i)
		{
			constantBufferManager->UpdatePointlights(i, lightManager->GetPointLightBufferData());

			transform.SetPosition(pointLightBuffer[i].position);
			transform.SetScale(pointLightBuffer[i].radius);

			renderer->RenderUnlitStaticModel(transform.GetMatrix(), mesh, shader.get(), texture.get());
		}

		stateManager->SetDepthStencilState(context, Graphics::eDepthStencilState::Less_Equal);
		stateManager->SetRasterizerState(context,previousRasterizerState);
		stateManager->SetBlendState(context, Graphics::eBlendState::Disabled);

		ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
		context->PSSetShaderResources(5, 5, nullSRVs);

		if (renderer->GetShouldRenderDebugLines() == true)
		{
			Drawer::Sphere pointLightDebugSpheres;
			pointLightDebugSpheres.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			for (size_t i = 0; i < lightManager->GetPointLightCount(); ++i)
			{
				pointLightDebugSpheres.position = pointLightBuffer[i].position;
				pointLightDebugSpheres.radius = pointLightBuffer[i].radius;
				renderer->Push(pointLightDebugSpheres);

				pointLightDebugSpheres.radius = 0.1f;
				renderer->Push(pointLightDebugSpheres);
			}
		}
	}

	std::unique_ptr<System> RenderLightSystem::Clone() const
	{
		return std::make_unique<RenderLightSystem>();
	}
}