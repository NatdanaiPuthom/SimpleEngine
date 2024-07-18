#include "Game/Precomplied/GamePch.hpp"
#include "Game/Systems/RewindSystem/RewindSystem.hpp"
#include "Game/Components/RewindTestComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"

namespace ECS
{
	RewindSystem::RewindSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	RewindSystem::~RewindSystem()
	{
	}

	void RewindSystem::GoToPoint(ECS::TransformComponent* aTransformComponent, ECS::RewindTestComponent* aRewindTestComponent)
	{

		Math::Vector3f position = aTransformComponent->transform.GetPosition();

		Math::Vector3f direction;

		Math::Vector3f wayPoint;


		bool goToFirstWayPoint = aRewindTestComponent->goToFirstPoint;

		if (goToFirstWayPoint)
		{
			direction = aRewindTestComponent->wayPoint1 - position;
			wayPoint = aRewindTestComponent->wayPoint1;
		}
		else
		{
			direction = aRewindTestComponent->wayPoint2 - position;
			wayPoint = aRewindTestComponent->wayPoint2;
		}

		direction.Normalize();

		Math::Vector3f velocity = direction * 0.5f * Global::GetDeltaTime();

		position += velocity;

		aTransformComponent->transform.SetPosition(position);

		Math::Vector3f rotation = aTransformComponent->transform.GetRotation();

		rotation += aRewindTestComponent->addRotation * 8.0f * Global::GetDeltaTime();

		aTransformComponent->transform.SetRotation(rotation);

		float distance = Math::Distance(position, wayPoint);

		if (distance <= velocity.Length())
		{
			aRewindTestComponent->goToFirstPoint = !aRewindTestComponent->goToFirstPoint;
		}

	}

	void RewindSystem::Init()
	{
	
	}

	void RewindSystem::Update()
	{
		auto& RewindComponentsIDs = myEntityComponentSystem->GetEntityIDsWithThisComponent<RewindTestComponent>();

		for (auto& thisRewindComponentID : RewindComponentsIDs)
		{
			auto& thisEntity = myEntityComponentSystem->GetEntity(thisRewindComponentID);

			//auto& thisEntityTransformComponent = thisEntity->GetComponent<TransformComponent>();

			ECS::TransformComponent* thisEntityTransformComponent = thisEntity->GetComponent<TransformComponent>();

			ECS::RewindTestComponent* thisRewindComponent = thisEntity->GetComponent<RewindTestComponent>();


			GoToPoint(thisEntityTransformComponent, thisRewindComponent);

			/*if (thisRewindComponent->goToFirstPoint == true)
			{
				Math::Vector3f position = thisEntityTransformComponent->transform.GetPosition();

				Math::Vector3f direction = thisRewindComponent->wayPoint1 - position;

				direction.Normalize();

				position += direction * 0.5f * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetPosition(position);

				Math::Vector3f rotation = thisEntityTransformComponent->transform.GetRotation();

				rotation += thisRewindComponent->addRotation * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetRotation(rotation);

				float distance = Math::Distance(position, thisRewindComponent->wayPoint1);

				if (distance <= Math::Vector3f(direction * 0.5f * Global::GetDeltaTime()).Length())
				{
					thisRewindComponent->goToFirstPoint = false;
				}
			}
			else
			{

				Math::Vector3f position = thisEntityTransformComponent->transform.GetPosition();

				Math::Vector3f direction = thisRewindComponent->wayPoint2 - position;

				direction.Normalize();

				position += direction * 0.5f * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetPosition(position);

				Math::Vector3f rotation = thisEntityTransformComponent->transform.GetRotation();

				rotation += thisRewindComponent->addRotation * Global::GetDeltaTime();

				thisEntityTransformComponent->transform.SetRotation(rotation);

				float distance = Math::Distance(position, thisRewindComponent->wayPoint2);

				if (distance <= Math::Vector3f(direction * 0.5f * Global::GetDeltaTime()).Length())
				{
					thisRewindComponent->goToFirstPoint = true;
				}

			}*/


		}


	}

	std::unique_ptr<System> RewindSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RewindSystem>(aEntityComponentSystem);
	}
}