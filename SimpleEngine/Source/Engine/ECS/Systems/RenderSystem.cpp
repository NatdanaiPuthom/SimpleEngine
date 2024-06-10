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

		ECS::Entities entities = myEntityManager->GetAllEntities();

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

			renderer->RenderStaticModel(transform, mesh);
		}
	}
}