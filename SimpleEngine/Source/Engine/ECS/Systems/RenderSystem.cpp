#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/AnimatedComponent.hpp"

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
		const ECS::Entities entities = myEntityComponentSystem->GetAllEntities();
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

			if (mesh->textures[0] == nullptr)
			{
				mesh->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			}

			if (mesh->shader == nullptr)
			{
				mesh->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			}
		}
	}

	void RenderSystem::Render()
	{
		const Drawer::Renderer* renderer = Global::GetRenderer();
		const ECS::Entities entities = myEntityComponentSystem->GetAllEntities();

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			const MeshComponent* mesh = entities[i]->GetComponent<ECS::MeshComponent>(); //To-DO(v9.37.2): Disgusting, fix pls
			const TransformComponent* transform = entities[i]->GetComponent<ECS::TransformComponent>(); //TO-DO(v9.37.2): Disgusting, fix pls

			if (mesh == nullptr || transform == nullptr)
			{
				continue;
			}
			else if (mesh->textures[0] == nullptr)
			{
				continue;
			}

			const AnimatedComponent* animated = entities[i]->GetComponent<ECS::AnimatedComponent>();

			if (animated != nullptr)
			{
				if (animated->skeleton != nullptr && animated->shader != nullptr)
				{
					renderer->RenderAnimatedModel(transform, mesh, animated);
				}
				else
				{
					renderer->RenderStaticModel(transform, mesh);
				}
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