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
			const bool isUsingPBR = Global::GetGraphicsEngine()->IsUsingPBR();

			if (animated != nullptr && animated->skeleton != nullptr && animated->shader != nullptr)
			{
				if (isUsingPBR == true)
				{
					renderer->RenderPBRAnimatedModel(transform, mesh, animated);
				}
				else
				{
					myAnimatedModelToRender.emplace_back(AnimatedModelToRender(transform, mesh, animated)); //NOTE(v11.3.2): Because deferred rendering so it has to be rendered after lightning pass, it works but may need to refactor in future
				}
			}
			else
			{
				if (isUsingPBR == true)
				{
					renderer->RenderPBRStaticModel(transform, mesh);
				}
				else
				{
					myStaticModelToRender.emplace_back(StaticModelToRender(transform, mesh)); //NOTE(v11.3.2): Because deferred rendering so it has to be rendered after lightning pass, it works but may need to refactor in future
				}
			}
		}
	}

	void RenderSystem::LateRender()
	{
		RenderUnlitModels();
	}

	std::unique_ptr<System> RenderSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RenderSystem>(aEntityComponentSystem);
	}

	void RenderSystem::RenderUnlitModels()
	{
		const Drawer::Renderer* renderer = Global::GetRenderer();

		for (size_t i = 0; i < myStaticModelToRender.size(); ++i)
		{
			const TransformComponent* transform = myStaticModelToRender[i].transformComponent;
			const MeshComponent* mesh = myStaticModelToRender[i].meshComponent;
			renderer->RenderUnlitStaticModel(transform->transform.GetMatrix(), mesh->mesh, mesh->shader, mesh->textures[Graphics::Global_Slot_Albedo]);
		}

		for (size_t i = 0; i < myAnimatedModelToRender.size(); ++i)
		{
			const TransformComponent* transform = myAnimatedModelToRender[i].transformComponent;
			const MeshComponent* mesh = myAnimatedModelToRender[i].meshComponent;
			const AnimationComponent* animated = myAnimatedModelToRender[i].animationComponent;
			renderer->RenderUnlitStaticAnimatedModel(transform, mesh, animated);
		}

		myStaticModelToRender.clear();
		myAnimatedModelToRender.clear();
	}
}