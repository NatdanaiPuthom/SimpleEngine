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
	}

	void RenderSystem::Render()
	{
		ECS::Entities entities = myEntityManager->GetAllEntities();

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			auto mesh = entities[i]->GetComponent<ECS::MeshComponent>();

			if (mesh == nullptr || mesh->mesh == nullptr || mesh->texture == nullptr)
			{
				continue;
			}

			auto transform = entities[i]->GetComponent<ECS::TransformComponent>();
			Global::GetRenderer()->RenderStaticModel(transform, mesh);
		}
	}
}