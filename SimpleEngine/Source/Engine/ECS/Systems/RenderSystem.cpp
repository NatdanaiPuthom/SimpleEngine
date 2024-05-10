#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"

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
				mesh->texture = graphicsEngine->GetDefaultTexture().get();
			}
		}
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