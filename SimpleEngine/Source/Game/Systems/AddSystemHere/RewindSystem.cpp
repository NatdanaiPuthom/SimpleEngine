#include "Game/Precomplied/GamePch.hpp"
#include "RewindSystem.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Game/Components/AddComponentHere/RewindTestComponent.hpp"


namespace ECS
{
	RewindSystem::RewindSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	RewindSystem::~RewindSystem()
	{
	}

	void RewindSystem::Init()
	{
		std::cout << "Initialized RewindSystem!" << std::endl;
	}

	void RewindSystem::Update()
	{
		auto& RewindComponentsIDs = myEntityComponentSystem->GetEntityIDsWithThisComponent<RewindTestComponent>();

		for (auto& thisRewindComponentID : RewindComponentsIDs)
		{
			auto& thisEntity = myEntityComponentSystem->GetEntity(thisRewindComponentID);

			auto& thisEntityTransformComponent = thisEntity->GetComponent<TransformComponent>();

			auto& thisRewindComponent = thisEntity->GetComponent<RewindTestComponent>();


			if (thisRewindComponent->goToFirstPoint == true)
			{
				Math::Vector3f position = thisEntityTransformComponent->transform.GetPosition();

				Math::Vector3f destination = thisRewindComponent->wayPoint1 - position;

				destination.Normalize();

				position += destination * 0.5f * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetPosition(position);

				Math::Vector3f rotation = thisEntityTransformComponent->transform.GetRotation();

				rotation += thisRewindComponent->addRotation * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetRotation(rotation);


			}


		}


	}

	void RewindSystem::Render()
	{
		std::cout << "RewindSystem is begin rendered!" << std::endl;
	}

	void RewindSystem::EarlyUpdate()
	{
		std::cout << "RewindSystem is begin early updated!" << std::endl;
	}

	void RewindSystem::FixedUpdate()
	{
		std::cout << "RewindSystem is begin fixed updated!" << std::endl;
	}

	void RewindSystem::LateUpdate()
	{
		std::cout << "RewindSystem is begin late updated!" << std::endl;
	}

	std::unique_ptr<System> RewindSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RewindSystem>(aEntityComponentSystem);
	}
}