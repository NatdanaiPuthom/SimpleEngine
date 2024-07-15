#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/AnimationComponent.hpp"

namespace ECS
{
	RenderSystem::RenderSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
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
		const Drawer::Renderer* renderer = Global::GetRenderer();
		const ECS::Entities entities = myEntityComponentSystem->GetAllEntities();

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			const ECS::Entity entity = entities[i];

			const MeshComponent* mesh = entity->GetComponent<ECS::MeshComponent>(); //To-DO(v9.37.2): Disgusting, fix pls
			const TransformComponent* transform = entity->GetComponent<ECS::TransformComponent>(); //TO-DO(v9.37.2): Disgusting, fix pls

			if (mesh == nullptr || transform == nullptr)
			{
				continue;
			}

			const AnimationComponent* animated = entity->GetComponent<ECS::AnimationComponent>();

			if (animated != nullptr && animated->skeleton != nullptr && animated->shader != nullptr)
			{
				renderer->RenderAnimatedModel(transform, mesh, animated);
			}
			else
			{
				renderer->RenderStaticModel(transform, mesh);
			}
		}
	}

	std::unique_ptr<System> RenderSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RenderSystem>(aEntityComponentSystem);
	}
}