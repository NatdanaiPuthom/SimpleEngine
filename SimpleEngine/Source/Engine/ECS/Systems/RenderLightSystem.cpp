#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/SkyBoxComponent.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"
#include "External/imgui.h"

namespace ECS
{
	RenderLightSystem::RenderLightSystem(EntityManager* aECS) : System(aECS)
		, mySkyBoxID(static_cast<size_t>(-1))
		, myDirectionalLightID(static_cast<size_t>(-1))
	{
	}

	RenderLightSystem::~RenderLightSystem()
	{
	}

	void RenderLightSystem::Init()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		{
			ECS::Entity entity = myEntityManager->CreateEntity();
			entity->SetName("SkyBox");
			entity->AddComponent<SkyBoxComponent>();

			SkyBoxComponent* skyBoxComponent = entity->GetComponent<SkyBoxComponent>();
			skyBoxComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::SkyBox).get();
			skyBoxComponent->texture = graphicsEngine->GetTexture(Graphics::eTextureType::SkyBox_AutumnForest).get();
			skyBoxComponent->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::SkyBox);

			mySkyBoxID = entity->GetID();
		}

		{
			ECS::Entity directionalLight = myEntityManager->CreateEntity();
			directionalLight->SetName("Directional Light");
			directionalLight->AddComponent<DirectionalLight>();

			DirectionalLight* directionalLightComponent = directionalLight->GetComponent<DirectionalLight>();
			directionalLightComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			directionalLightComponent->texture = graphicsEngine->GetTexture("Assets\\Textures\\Sunlight.dds").get();
			directionalLightComponent->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			directionalLightComponent->transform.SetPosition({ 0.0f, 5.0f, 0.0f });

			myDirectionalLightID = directionalLight->GetID();
		}
	}

	void RenderLightSystem::Update()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		ECS::Entity skyboxEntity = myEntityManager->GetEntity(mySkyBoxID);
		ECS::Entity directionalLight = myEntityManager->GetEntity(myDirectionalLightID);

		SkyBoxComponent* skyBoxComponent = skyboxEntity->GetComponent<SkyBoxComponent>();
		DirectionalLight* directionalLightComponent = directionalLight->GetComponent<DirectionalLight>();

		const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

		skyBoxComponent->transform.SetPosition(graphicsEngine->GetCurrentCamera()->GetPosition());
		graphicsEngine->SetDirectionalLightDirection((forward.GetNormalized()));

		{
			PointLightData pointLight1;
			pointLight1.color = { 1.0f, 1.0f, 1.0f, 50.0f };
			pointLight1.position = { -3.5f, 1.0f, 0.0f };
			pointLight1.radius = 1.5f;

			graphicsEngine->AddPointLight(pointLight1);

			PointLightData pointLight2;
			pointLight2.color = { 0.0f, 1.0f, 0.0f, 100.0f };
			pointLight2.position = { 0.0f, -1.0f, -1.0f };
			pointLight2.radius = 1.2f;

			graphicsEngine->AddPointLight(pointLight2);
		}
	}

	void RenderLightSystem::Render()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		ID3D11DeviceContext* context = graphicsEngine->GetContext().Get(); context;
		const Drawer::Renderer* renderer = Global::GetRenderer();

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

		Global::GetGraphicsEngine()->SetRasterizerState(Graphics::eRasterizerState::FrontFaceCulling);
		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Greater);
		graphicsEngine->SetBlendState(Graphics::eBlendState::AdditiveBlend);
		graphicsEngine->UpdateLightBuffer();

		PointLightData* pointLightBuffer = graphicsEngine->GetPointLightDataArray();

		for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
		{
			transform.SetPosition(pointLightBuffer[i].position);
			transform.SetScale(pointLightBuffer[i].radius);

			renderer->RenderUnlit(transform.GetMatrix(), mesh, shader.get(), texture.get());
		}

		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Less_Equal);
		graphicsEngine->SetRasterizerState(Graphics::eRasterizerState::BackfaceCulling);
		graphicsEngine->SetBlendState(Graphics::eBlendState::Disabled);

		ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
		context->PSSetShaderResources(5, 5, nullSRVs);

		Drawer::Sphere pointLightDebugSpheres;
		pointLightDebugSpheres.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
		{
			pointLightDebugSpheres.position = pointLightBuffer[i].position;
			pointLightDebugSpheres.radius = pointLightBuffer[i].radius;
			renderer->RenderSphere(pointLightDebugSpheres);
		}

		{
			ECS::Entity directionalLight = myEntityManager->GetEntity(myDirectionalLightID);
			DirectionalLight* directionalLightComponent = directionalLight->GetComponent<DirectionalLight>();

			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

			Drawer::Line line;
			line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			line.startPosition = directionalLightComponent->transform.GetPosition();
			line.endPosition = line.startPosition + forward * 5.0f;

			Drawer::Sphere sphere;
			sphere.radius = 0.25f;
			sphere.position = line.endPosition;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			renderer->RenderSphere(sphere);
			renderer->RenderLine(line);
		}
	}

	void RenderLightSystem::RenderSkyBoxAndDirectionalLight()
	{
		const Drawer::Renderer* renderer = Global::GetRenderer();

		ECS::Entity skyBox = myEntityManager->GetEntity(mySkyBoxID);
		ECS::Entity directionalLight = myEntityManager->GetEntity(myDirectionalLightID);

		const SkyBoxComponent* skyBoxComponent = skyBox->GetComponent<SkyBoxComponent>();
		const DirectionalLight* directionalLightComponent = directionalLight->GetComponent<DirectionalLight>();

		renderer->RenderUnlit(skyBoxComponent->transform.GetMatrix(), skyBoxComponent->mesh, skyBoxComponent->shader, skyBoxComponent->texture);
		renderer->RenderUnlit(directionalLightComponent->transform.GetMatrix(), directionalLightComponent->mesh, directionalLightComponent->shader, directionalLightComponent->texture);
	}
}