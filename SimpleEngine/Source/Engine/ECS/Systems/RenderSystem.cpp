#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/AnimationComponent.hpp"

namespace ECS
{
	RenderSystem::RenderSystem()
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::Render(EntityComponentSystem* aEntityComponentSystem)
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		Drawer::Renderer* renderer = graphicsEngine->GetRenderer();

		const std::unordered_set<EntityID>& entitiesWithSkyBoxComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<SkyBoxComponent>();
		const std::unordered_set<EntityID>& entitiesWithDirectionalLightComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<DirectionalLightComponent>();

		if (entitiesWithSkyBoxComponent.empty())
		{
			static ID3D11ShaderResourceView* nullview[1] = { nullptr };
			graphicsEngine->GetContext()->PSSetShaderResources(static_cast<unsigned int>(Graphics::Global_Slot_CubeMap), 1, nullview);
		}

		if (entitiesWithDirectionalLightComponent.empty() == false)
		{
			ECS::Entity& directionalLight = aEntityComponentSystem->GetEntity(*entitiesWithDirectionalLightComponent.begin());
			const DirectionalLightComponent* directionalLightComponent = directionalLight.GetComponent<DirectionalLightComponent>();
			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

			graphicsEngine->SetDirectionalLightDirection(forward.GetNormalized() * -1.0f);
			graphicsEngine->SetDirectionalLightColor({ 1.0f, 1.0f, 1.0f,1.0f }); //TO-DO(v11.1.0): add color to the component to retrieve data instead of hardcoded
		}
		else
		{
			graphicsEngine->SetDirectionalLightDirection({ 0.0f,0.0f, -1.0f });
			graphicsEngine->SetDirectionalLightColor({ 0.4f, 0.4f, 0.4f,0.4f });
		}

		std::vector<Entity>& entities = aEntityComponentSystem->GetAllEntities();
		const bool shouldRenderMesh = renderer->GetShouldRenderMesh();

		for (size_t i = 0; i < entities.size(); ++i)
		{
			 ECS::Entity& entity = entities[i];
			const MeshComponent* mesh = entity.GetComponent<ECS::MeshComponent>(); //To-DO(v9.37.2): Disgusting, fix pls
			const TransformComponent* transform = entity.GetComponent<ECS::TransformComponent>(); //TO-DO(v9.37.2): Disgusting, fix pls

			if (mesh == nullptr || transform == nullptr)
			{
				continue;
			}

			const AnimationComponent* animated = entity.GetComponent<ECS::AnimationComponent>();
			const bool isUsingPBR = renderer->GetIsUsingPBR();

			if (renderer->GetShouldRenderBoundingBox() == true)
			{
				renderer->Push(Drawer::BoundingBox3DData(transform->transform.GetMatrix(), mesh->mesh->GetBoundingBox()));
			}

			if (animated != nullptr && animated->skeleton != nullptr && animated->shader != nullptr)
			{
				if (renderer->GetShouldRenderSkeletonLines() == true)
				{
					if (animated->animationPlayer.myModelSpacePose.count > 0)
					{
						renderer->RenderAnimatedSkeletonLines(transform, animated);
					}
					else
					{
						renderer->RenderStaticSkeletonLines(transform, animated);
					}
				}

				if (shouldRenderMesh == false)
				{
					continue;
				}

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
				if (shouldRenderMesh == false)
				{
					continue;
				}

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

	void RenderSystem::LateRender(EntityComponentSystem* aEntityComponentSystem)
	{
		RenderUnlitModels(aEntityComponentSystem);
	}

	std::unique_ptr<System> RenderSystem::Clone() const
	{
		return std::make_unique<RenderSystem>();
	}

	void RenderSystem::RenderUnlitModels(EntityComponentSystem* aEntityComponentSystem)
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		Drawer::Renderer* renderer = graphicsEngine->GetRenderer();

		const Graphics::Shader* unlitShader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
		const std::unordered_set<EntityID>& entitiesWithSkyBoxComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<SkyBoxComponent>();
		const std::unordered_set<EntityID>& entitiesWithDirectionalLightComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<DirectionalLightComponent>();

		for (size_t i = 0; i < myStaticModelToRender.size(); ++i)
		{
			const TransformComponent* transform = myStaticModelToRender[i].transformComponent;
			const MeshComponent* mesh = myStaticModelToRender[i].meshComponent;
			renderer->RenderUnlitStaticModel(transform->transform.GetMatrix(), mesh->mesh, unlitShader, mesh->textures[Graphics::Global_Slot_Albedo]);
		}

		for (size_t i = 0; i < myAnimatedModelToRender.size(); ++i)
		{
			const TransformComponent* transform = myAnimatedModelToRender[i].transformComponent;
			const MeshComponent* mesh = myAnimatedModelToRender[i].meshComponent;
			const AnimationComponent* animated = myAnimatedModelToRender[i].animationComponent;
			renderer->RenderUnlitStaticAnimatedModel(transform, mesh, animated);
		}

		if (entitiesWithSkyBoxComponent.empty() == false)
		{
			ECS::Entity skyBox = aEntityComponentSystem->GetEntity(*entitiesWithSkyBoxComponent.begin());
			SkyBoxComponent* skyBoxComponent = skyBox.GetComponent<SkyBoxComponent>();
			skyBoxComponent->transform.SetPosition(graphicsEngine->GetCurrentCamera()->GetPosition());

			renderer->RenderUnlitStaticModel(skyBoxComponent->transform.GetMatrix(), skyBoxComponent->mesh, skyBoxComponent->shader, skyBoxComponent->texture);
		}

		if (entitiesWithDirectionalLightComponent.empty() == false)
		{
			ECS::Entity directionalLight = aEntityComponentSystem->GetEntity(*entitiesWithDirectionalLightComponent.begin());
			const DirectionalLightComponent* directionalLightComponent = directionalLight.GetComponent<DirectionalLightComponent>();
			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

			Drawer::Line line;
			line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			line.startPosition = directionalLightComponent->transform.GetPosition();
			line.endPosition = line.startPosition + forward * 5.0f;

			Drawer::Sphere sphere;
			sphere.radius = 0.25f;
			sphere.position = line.endPosition;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			renderer->Push(sphere);
			renderer->Push(line);
			renderer->RenderUnlitStaticModel(directionalLightComponent->transform.GetMatrix(), directionalLightComponent->mesh, directionalLightComponent->shader, directionalLightComponent->texture);
		}

		PROFILER_FUNCTION(profiler::colors::Red);
		PROFILER_BEGIN("Render Debug Lines");
		if (renderer->GetShouldRenderDebugLines() == true)
		{
			graphicsEngine->GetRenderer()->RenderDebugLines();
		}
		PROFILER_END();

		myStaticModelToRender.clear();
		myAnimatedModelToRender.clear();
	}
}