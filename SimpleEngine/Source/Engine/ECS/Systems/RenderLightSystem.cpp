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
	}

	void RenderLightSystem::Render()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		const Drawer::Renderer* renderer = Global::GetRenderer();

		static Math::Vector4f pointLightColorAndIntensity1 = { 1.0f, 1.0f, 1.0f, 100.0f };
		static Math::Vector3f pointLightPosition = { -5.0f, 5.5f, 4.2f };
		static float range = 1.0f;

		ID3D11DeviceContext* context = Global::GetGraphicsEngine()->GetContext().Get(); context;

		PointLightData pointlight = graphicsEngine->GetPointLightData(0);
		pointlight.color = pointLightColorAndIntensity1;
		pointlight.position = pointLightPosition;
		pointlight.range = range;

		Drawer::Sphere pointLightSphere;
		pointLightSphere.radius = pointlight.range;
		pointLightSphere.position = pointlight.position;
		pointLightSphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		graphicsEngine->AddPointLight(pointlight);

		renderer->RenderSphere(pointLightSphere);

		std::vector<PointLightData> pointLights;

		for (size_t i = 0; i < 10; ++i)
		{
			PointLightData pointLight2;
			pointLight2.color = { 1.0f, 0.0f, 0.0f, 1000.0f };
			pointLight2.position = { i * 5.0f, -1.4f, 0.0f };
			pointLight2.range = range;
			pointLights.push_back(pointLight2);
		}

		for (size_t i = 0; i < pointLights.size(); ++i)
		{
			Drawer::Sphere pointLightSpheres;
			pointLightSpheres.radius = pointLights[i].range;
			pointLightSpheres.position = pointLights[i].position;
			pointLightSpheres.color = pointLights[i].color;
			renderer->RenderSphere(pointLightSpheres);
		}

		std::vector<Graphics::RenderTarget>& gBuffers = graphicsEngine->GetRenderTargets(Graphics::eRenderTargetType::GBuffer);

		ID3D11ShaderResourceView* shaderResources[5] = {};

		for (size_t i = 0; i < gBuffers.size(); ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		context->PSSetShaderResources(5, 5, shaderResources);

		Math::Transform transform;
		transform.SetPosition(pointLightPosition);
		transform.SetScale(range);

		const Graphics::Mesh* mesh = Global::GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Sphere);
		std::shared_ptr<const Graphics::Shader> shader = Global::GetGraphicsEngine()->GetShader("PointLightCullPS.cso", "DefaultVS.cso");
		std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTexture(Graphics::eTextureType::Default);

		Global::GetGraphicsEngine()->SetRasterizerState(Graphics::eRasterizerState::FrontFaceCulling);
		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Greater);
		graphicsEngine->SetBlendState(Graphics::eBlendState::AdditiveBlend);

		renderer->RenderUnlit(transform.GetMatrix(), mesh, shader.get(), texture.get());

		Math::Transform transform2;

		for (size_t i = 0; i < pointLights.size(); ++i)
		{
			graphicsEngine->AddPointLight(pointLights[i]);

			const Graphics::Mesh* mesh2 = Global::GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Sphere);
			std::shared_ptr<const Graphics::Shader> shader2 = Global::GetGraphicsEngine()->GetShader("PointLightCullPS.cso", "DefaultVS.cso");
			std::shared_ptr<const Graphics::Texture> texture2 = Global::GetGraphicsEngine()->GetTexture(Graphics::eTextureType::Default);
			transform2.SetPosition(pointLights[i].position);
			transform2.SetScale(pointLights[i].range);
			renderer->RenderUnlit(transform2.GetMatrix(), mesh2, shader2.get(), texture2.get());
		}

		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Less_Equal);
		graphicsEngine->SetRasterizerState(Graphics::eRasterizerState::BackfaceCulling);
		graphicsEngine->SetBlendState(Graphics::eBlendState::Disabled);

		ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
		context->PSSetShaderResources(5, 5, nullSRVs);


		/*{
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

		if (ImGui::Begin("Light"))
		{
			ImGui::Separator();

			if (ImGui::DragFloat4("PointLight1 Color", &pointLightColorAndIntensity1.x, 0.001f, 0.f, 1.f))
			{
			}

			if (ImGui::DragFloat4("PointLight1 Position", &pointLightPosition.x, 0.1f))
			{
			}

			if (ImGui::DragFloat("PointLight1 Range", &range, 0.1f, 0.f, FLT_MAX))
			{
			}
		}
		ImGui::End();*/
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