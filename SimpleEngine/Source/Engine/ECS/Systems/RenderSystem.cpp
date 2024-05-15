#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace ECS
{
	RenderSystem::RenderSystem(EntityManager* aECS) : System(aECS)
		, myEntityID(static_cast<size_t>(-1))
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::Init()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		{
			ECS::Entity entity = myEntityManager->CreateEntity();
			entity->SetName("SkyBox");

			entity->AddComponent<TransformComponent>();
			entity->AddComponent<MeshComponent>();

			entity->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::SkyBox).get();
			entity->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::SkyBox_DayCloud).get();
			entity->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::SkyBox);
		}

		{
			ECS::Entity floor = myEntityManager->CreateEntity();
			floor->SetName("Floor");

			floor->AddComponent<TransformComponent>();
			floor->AddComponent<MeshComponent>();

			floor->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			floor->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			floor->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			floor->GetComponent<TransformComponent>()->transform.SetPosition({ 15.0f, -2.0f, 12.0f });
			floor->GetComponent<TransformComponent>()->transform.SetScale({ 20.0f, 1.0f, 20.0f });
		}

		{
			ECS::Entity directionalLight = myEntityManager->CreateEntity();
			directionalLight->SetName("Directional Light");
			directionalLight->AddComponent<TransformComponent>();
			directionalLight->AddComponent<MeshComponent>();

			directionalLight->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			directionalLight->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture("Assets\\Textures\\Sunlight.dds").get();
			directionalLight->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			directionalLight->GetComponent<TransformComponent>()->transform.SetPosition({ 0.0f, 5.0f, 0.0f });

			myEntityID = directionalLight->GetID();
		}


		for (size_t i = 0; i < 10; ++i)
		{
			ECS::Entity e = myEntityManager->CreateEntity();
			e->SetName("Horizontal Cube " + std::to_string(i));

			e->AddComponent<TransformComponent>();
			e->AddComponent<MeshComponent>();

			e->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::PBR_Default).get();
			e->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			e->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			e->GetComponent<TransformComponent>()->transform.SetPosition({ 0.0f + i * 3.0f, 0.0f, 0.0f });
		}

		for (size_t i = 0; i < 10; ++i)
		{
			ECS::Entity e = myEntityManager->CreateEntity();
			e->SetName("Vertical Cube " + std::to_string(i));

			e->AddComponent<TransformComponent>();
			e->AddComponent<MeshComponent>();

			e->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::PBR_Default).get();
			e->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			e->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			e->GetComponent<TransformComponent>()->transform.SetPosition({ 15.0f, 0.0f, 0.0f + i * 3.0f });
		}
	}

	void RenderSystem::Update()
	{
		ECS::Entities entities = myEntityManager->GetAllEntities();
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			MeshComponent* mesh = entities[i]->GetComponent<ECS::MeshComponent>();

			if (mesh == nullptr)
			{
				continue;
			}

			if (mesh->mesh == nullptr)
			{
				mesh->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			}

			if (mesh->texture == nullptr)
			{
				mesh->texture = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			}

			if (mesh->shader == nullptr)
			{
				mesh->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			}
		}

		ECS::Entity e = myEntityManager->GetEntity(myEntityID);
		ECS::TransformComponent* t = e->GetComponent<TransformComponent>();
		const Math::Vector3f forward = t->transform.GetMatrix().GetForward();
		graphicsEngine->SetDirectionalLightDirection((forward.GetNormalized()));
	}

	void RenderSystem::Render()
	{
		ECS::Entities entities = myEntityManager->GetAllEntities();

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			const MeshComponent* mesh = entities[i]->GetComponent<ECS::MeshComponent>();

			if (mesh == nullptr)
			{
				continue;
			}

			const TransformComponent* transform = entities[i]->GetComponent<ECS::TransformComponent>();

			if (transform == nullptr)
			{
				continue;
			}

			Global::GetRenderer()->RenderStaticModel(transform, mesh);
		}


		ECS::Entity e = myEntityManager->GetEntity(myEntityID);

		const ECS::TransformComponent* t = e->GetComponent<TransformComponent>();
		const Math::Vector3f forward = t->transform.GetMatrix().GetForward();

		Drawer::Line line;
		line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		line.startPosition = t->transform.GetPosition();
		line.endPosition = line.startPosition + forward * 5.0f;

		Drawer::Sphere sphere;
		sphere.radius = 0.25f;
		sphere.position = line.endPosition;
		sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		Global::GetRenderer()->RenderSphere(sphere);
		Global::GetRenderer()->RenderLine(line);
	}
}