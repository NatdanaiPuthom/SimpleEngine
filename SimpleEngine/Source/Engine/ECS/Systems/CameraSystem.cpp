#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/CameraSystem.hpp"
#include "Engine/ECS/Components/Core/CameraComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace ECS
{
	CameraSystem::CameraSystem()
	{
	}

	CameraSystem::~CameraSystem()
	{
	}

	void CameraSystem::Render(EntityComponentSystem* aEntityComponentSystem)
	{

		if (MainSingleton::GetSceneManager().GetIsPlaying() == false) //TO-DO(v11.4.1): Better architecture/flow
		{
			return;
		}

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		const std::unordered_set<EntityID>& entitiesIDWithCameraComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<CameraComponent>();

		if (entitiesIDWithCameraComponent.empty())
		{
			graphicsEngine->SetCamera(graphicsEngine->GetEditorCamera().get());
			Simple::Console::Print("No Camera found in current scene", Simple::ConsoleTextColor::Red);
		}
		else
		{
			ECS::Entity& cameraEntity = aEntityComponentSystem->GetEntity(*entitiesIDWithCameraComponent.begin());
			CameraComponent* cameraComponent = cameraEntity.GetComponent<ECS::CameraComponent>();
			const TransformComponent* transform = cameraEntity.GetComponent<ECS::TransformComponent>();

			cameraComponent->camera.SetPosition(transform->transform.GetPosition());
			cameraComponent->camera.SetRotation(transform->transform.GetRotation());
			graphicsEngine->SetCamera(&cameraComponent->camera);
		}
	}

	void CameraSystem::LateRender(EntityComponentSystem* aEntityComponentSystem)
	{
		if (MainSingleton::GetSceneManager().GetIsPlaying() == true)
		{
			return;
		}

		const std::unordered_set<EntityID>& entitiesIDWithCameraComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<CameraComponent>();

		if (entitiesIDWithCameraComponent.empty() == false)
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			Drawer::Renderer* renderer = graphicsEngine->GetRenderer();

			const ECS::Entity& entity = aEntityComponentSystem->GetEntity(*entitiesIDWithCameraComponent.begin());
			const CameraComponent* camera = entity.GetComponent<ECS::CameraComponent>();
			const TransformComponent* transform = entity.GetComponent<ECS::TransformComponent>();
			const Math::Vector3f forward = camera->camera.GetMatrix().GetForward();

			Drawer::Line line;
			line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			line.startPosition = transform->transform.GetPosition();
			line.endPosition = line.startPosition + forward * 1.0f;

			Drawer::Sphere sphere;
			sphere.radius = 0.10f;
			sphere.position = line.endPosition;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			Drawer::Sphere sphere2;
			sphere2.radius = 0.25f;
			sphere2.position = line.startPosition;
			sphere2.color = { 1.0f, 1.0f, 0.0f, 1.0f };

			renderer->Push(sphere);
			renderer->Push(sphere2);
			renderer->Push(line);
		}
	}

	std::unique_ptr<System> CameraSystem::Clone() const
	{
		return std::unique_ptr<CameraSystem>();
	}
}