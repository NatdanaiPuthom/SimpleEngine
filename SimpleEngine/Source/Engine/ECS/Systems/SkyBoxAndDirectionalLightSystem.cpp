#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/SkyBoxAndDirectionalLightSystem.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/SkyBoxComponent.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"

namespace ECS
{
	SkyBoxAndDirectionalLightSystem::SkyBoxAndDirectionalLightSystem(EntityManager* aECS) : System(aECS)
		, mySkyBoxID(static_cast<size_t>(-1))
		, myDirectionalLightID(static_cast<size_t>(-1))
	{
	}

	SkyBoxAndDirectionalLightSystem::~SkyBoxAndDirectionalLightSystem()
	{
	}

	void SkyBoxAndDirectionalLightSystem::Init()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		{
			ECS::Entity entity = myEntityManager->CreateEntity();
			entity->SetName("SkyBox");
			entity->AddComponent<SkyBoxComponent>();

			SkyBoxComponent* skyBoxComponent = entity->GetComponent<SkyBoxComponent>();
			skyBoxComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::SkyBox).get();
			skyBoxComponent->texture = graphicsEngine->GetTexture(Graphics::eTextureType::SkyBox_DayCloud).get();
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

	void SkyBoxAndDirectionalLightSystem::Update()
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

	void SkyBoxAndDirectionalLightSystem::Render()
	{
		const Drawer::Renderer* renderer = Global::GetRenderer();

		ECS::Entity skyBox = myEntityManager->GetEntity(mySkyBoxID);
		ECS::Entity directionalLight = myEntityManager->GetEntity(myDirectionalLightID);

		const SkyBoxComponent* skyBoxComponent = skyBox->GetComponent<SkyBoxComponent>();
		const DirectionalLight* directionalLightComponent = directionalLight->GetComponent<DirectionalLight>();

		renderer->RenderUnlit(skyBoxComponent->transform.GetMatrix(), skyBoxComponent->mesh, skyBoxComponent->shader, skyBoxComponent->texture);
		renderer->RenderUnlit(directionalLightComponent->transform.GetMatrix(), directionalLightComponent->mesh, directionalLightComponent->shader, directionalLightComponent->texture);

		{
			PointLightData pointLightData = Global::GetGraphicsEngine()->GetPointLightData(0);

			Drawer::Sphere sphere;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			sphere.position = pointLightData.position;
			sphere.radius = pointLightData.range;

			renderer->RenderSphere(sphere);
		}

		{
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
}