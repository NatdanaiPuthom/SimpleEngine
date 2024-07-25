#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/CameraSystem.hpp"
#include "Engine/ECS/Components/Core/CameraComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/Debugger/Console/Console.hpp"

namespace ECS
{
	CameraSystem::CameraSystem()
	{
	}

	CameraSystem::~CameraSystem()
	{
	}

	void CameraSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		const std::unordered_set<EntityID>& entitiesIDWithCameraComponent = aEntityComponentSystem->GetEntityIDsWithThisComponent<CameraComponent>();

		if (entitiesIDWithCameraComponent.empty())
		{
			graphicsEngine->SetCamera(graphicsEngine->GetEditorCamera().get());
			Simple::Console::Print("No Camera found in current scene", Simple::ConsoleTextColor::Red);
		}
		else
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(*entitiesIDWithCameraComponent.begin());
			CameraComponent* camera = entity.GetComponent<ECS::CameraComponent>();
			TransformComponent* transform = entity.GetComponent<ECS::TransformComponent>();

			camera->camera.SetPosition(transform->transform.GetPosition());
			camera->camera.SetRotation(transform->transform.GetRotation());
		}
	}

	std::unique_ptr<System> CameraSystem::Clone() const
	{
		return std::unique_ptr<CameraSystem>();
	}
}