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
		const Graphics::Shader* unlitShader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Unlit_Default).get();

		const std::unordered_set<EntityID>& entitiesWithSkyBoxComponent = myEntityComponentSystem->GetEntityIDsWithThisComponent<SkyBoxComponent>();
		const std::unordered_set<EntityID>& entitiesWithDirectionalLightComponent = myEntityComponentSystem->GetEntityIDsWithThisComponent<DirectionalLightComponent>();

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
			const ECS::Entity skyBox = myEntityComponentSystem->GetEntity(*entitiesWithSkyBoxComponent.begin());
			const SkyBoxComponent* skyBoxComponent = skyBox->GetComponent<SkyBoxComponent>();
			renderer->RenderUnlitStaticModel(skyBoxComponent->transform.GetMatrix(), skyBoxComponent->mesh, skyBoxComponent->shader, skyBoxComponent->texture);
		}

		if (entitiesWithDirectionalLightComponent.empty() == false)
		{
			const ECS::Entity directionalLight = myEntityComponentSystem->GetEntity(*entitiesWithDirectionalLightComponent.begin());
			const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
			renderer->RenderUnlitStaticModel(directionalLightComponent->transform.GetMatrix(), directionalLightComponent->mesh, directionalLightComponent->shader, directionalLightComponent->texture);
		}

		myStaticModelToRender.clear();
		myAnimatedModelToRender.clear();

		//if (FindAndSetSkyBox())
		//{
		//	const Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		//	const ECS::Entity skyboxEntity = myEntityComponentSystem->GetEntity(myEntityWithSkyBoxID);
		//	SkyBoxComponent* skyBoxComponent = skyboxEntity->GetComponent<SkyBoxComponent>();
		//	skyBoxComponent->transform.SetPosition(graphicsEngine->GetCurrentCamera()->GetPosition());
		//}

		//if (FindAndSetDirectionalLight())
		//{
		//	Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		//	const ECS::Entity directionalLight = myEntityComponentSystem->GetEntity(myEntityWithDirectionalLightID);
		//	const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
		//	const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();
		//	graphicsEngine->SetDirectionalLightDirection(forward.GetNormalized() * -1.0f);
		//	graphicsEngine->SetDirectionalLightColor({ 1.0f, 1.0f, 1.0f,1.0f }); //TO-DO(v11.1.0): add color to the component to retrieve data instead of hardcoded
		//}
		//else
		//{
		//	Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		//	graphicsEngine->SetDirectionalLightDirection({ 0.0f,0.0f, -1.0f });
		//	graphicsEngine->SetDirectionalLightColor({ 0.4f, 0.4f, 0.4f,0.4f });
		//}

		/*if (myEntityWithDirectionalLightID != (static_cast<size_t>(-1)))
		{
			const ECS::Entity directionalLight = myEntityComponentSystem->GetEntity(myEntityWithDirectionalLightID);
			const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

			Drawer::Line line;
			line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			line.startPosition = directionalLightComponent->transform.GetPosition();
			line.endPosition = line.startPosition + forward * 5.0f;

			Drawer::Sphere sphere;
			sphere.radius = 0.25f;
			sphere.position = line.endPosition;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			renderer->RenderSphere(sphere);
			renderer->RenderLine(line);
		}*/
	}
}