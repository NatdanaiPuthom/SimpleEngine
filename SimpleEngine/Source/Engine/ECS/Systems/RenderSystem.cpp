#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace ECS
{
	RenderSystem::RenderSystem(EntityManager* aECS) : System(aECS)
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
			floor->GetComponent<TransformComponent>()->transform.SetPosition({ 0.0f, -2.0f, 0.0f });
			floor->GetComponent<TransformComponent>()->transform.SetScale({ 50.0f, 1.0f, 50.0f });
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

		/*static Math::Vector3f dir = { 0.0f, 0.0f,0.0f };

		if (MainSingleton::GetInputManager().IsKeyHeld('E'))
		{
			dir.x += 10.0f * Global::GetDeltaTime();
		}

		if (MainSingleton::GetInputManager().IsKeyHeld('C'))
		{
			dir.y += 10.0f * Global::GetDeltaTime();
		}

		if (MainSingleton::GetInputManager().IsKeyHeld('D'))
		{
			dir.z += 10.0f * Global::GetDeltaTime();
		}*/

		graphicsEngine->SetDirectionalLightDirection(Math::Vector3f(45.0f, 0.0f, 0.0f));
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
	}
}