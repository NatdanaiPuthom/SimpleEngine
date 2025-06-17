#include "Game/Precomplied/GamePch.hpp"
#include "Game/ECS/Systems/AddSystems/BasicPlayerSystem.hpp"
#include "Game/ECS/Components/AddComponents/BasicPlayerComponent.hpp"

namespace ECS
{
	BasicPlayerSystem::BasicPlayerSystem()
	{
	}

	BasicPlayerSystem::~BasicPlayerSystem()
	{
	}

	void BasicPlayerSystem::Init(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}

	void BasicPlayerSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& playerEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<BasicPlayerComponent>();

		EntityID playerID = static_cast<size_t>(-1);

		if (!playerEntities.empty())
		{
			playerID = *playerEntities.begin();
		}
	
		Entity& playerEntity = aEntityComponentSystem->GetEntity(playerID);

		TransformComponent* transformComponent = playerEntity.GetComponent<TransformComponent>();
		BasicPlayerComponent* basicPlayerComponent = playerEntity.GetComponent<BasicPlayerComponent>();

		Math::Vector3f position = transformComponent->transform.GetPosition();
		Math::Vector3f rotation = transformComponent->transform.GetRotation();

		Math::Vector3f velocity = Math::Vector3f::Zero();

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_LEFT))
		{
			velocity.x += -1.0f;
			rotation.y = 90.0f;
		}

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_RIGHT))
		{
			velocity.x += 1.0f;
			rotation.y = -90.0f;
		}

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_UP))
		{
			velocity.z += 1.0f;
			rotation.y = 180.0f;
		}

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_DOWN))
		{
			velocity.z += -1.0f;
			rotation.y = 0.0f;
		}

		velocity.Normalize();

		position += velocity * basicPlayerComponent->moveSpeed * Global::GetDeltaTime();
		transformComponent->transform.SetPosition(position);
		transformComponent->transform.SetRotation(rotation);
	}

	void BasicPlayerSystem::Render(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}

	void BasicPlayerSystem::EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}

	void BasicPlayerSystem::FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}

	void BasicPlayerSystem::LateUpdate(EntityComponentSystem* aEntityComponentSystem)
	{
		const std::unordered_set<EntityID>& cameraEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<CameraComponent>();
		const std::unordered_set<EntityID>& playerEntities = aEntityComponentSystem->GetEntityIDsWithThisComponent<BasicPlayerComponent>();

		EntityID cameraID = static_cast<size_t>(-1);
		EntityID playerID = static_cast<size_t>(-1);

		if (!playerEntities.empty())
		{
			playerID = *playerEntities.begin();
		}

		if (!cameraEntities.empty())
		{
			cameraID = *cameraEntities.begin();
		}

		if (cameraID == static_cast<size_t>(-1) && playerID == static_cast<size_t>(-1))
		{
			return;
		}

		Entity& playerEntity = aEntityComponentSystem->GetEntity(playerID);
		Entity& cameraEntity = aEntityComponentSystem->GetEntity(cameraID);

		TransformComponent* playerTransformComponent = playerEntity.GetComponent<TransformComponent>();
		BasicPlayerComponent* basicPlayerComponent = playerEntity.GetComponent<BasicPlayerComponent>();

		const Math::Vector3f newCameraPosition = playerTransformComponent->transform.GetPosition() + basicPlayerComponent->cameraOffsetDistance;
		cameraEntity.GetComponent<TransformComponent>()->transform.SetPosition(newCameraPosition);
	}

	std::unique_ptr<System> BasicPlayerSystem::Clone() const
	{
		return std::make_unique<BasicPlayerSystem>();
	}
}