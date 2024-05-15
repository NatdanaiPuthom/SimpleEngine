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
		ECS::Entity entity = myEntityManager->CreateEntity();
		entity->AddComponent<TransformComponent>();
		entity->AddComponent<MeshComponent>();

		entity->SetName("SkyBox");

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		entity->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::SkyBox).get();
		entity->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::SkyBox_DayCloud).get();
		entity->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::SkyBox);


		ECS::Entity test = myEntityManager->CreateEntity();
		test->AddComponent<TransformComponent>();
		test->AddComponent<MeshComponent>();
		test->SetName("Test");

		test->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::PBR_Default).get();
		test->GetComponent<MeshComponent>()->texture = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
		test->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
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