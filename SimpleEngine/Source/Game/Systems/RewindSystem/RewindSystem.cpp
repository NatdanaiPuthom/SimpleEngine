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

	std::unique_ptr<System> RewindSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<RewindSystem>(aEntityComponentSystem);
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
		rewindTimeQue.reserve(600);
	}

	void RewindSystem::Update()
	{

		if (GetAsyncKeyState(VK_MBUTTON))
		{

			if (rewindTimeQue.size()<=0)
			{
				return;
			}

			std::unordered_map<ECS::EntityID, RewindDataToRightComponent>& rewindMap = rewindTimeQue.back();

			for (auto& [entity, structData] : rewindMap)
			{
				auto& thisEntity = myEntityComponentSystem->GetEntity(entity);

				thisEntity->GetComponent<TransformComponent>()->transform = structData.transform;;
				thisEntity->GetComponent<RewindTestComponent>()->goToFirstPoint = structData.ThisBoolCheck;
			}
			rewindTimeQue.pop_back();


		}
		else
		{
			auto& RewindComponentsIDs = myEntityComponentSystem->GetEntityIDsWithThisComponent<RewindTestComponent>();


			std::unordered_map<ECS::EntityID, RewindDataToRightComponent> rewindMap;

			for (auto& thisRewindComponentID : RewindComponentsIDs)
			{
				auto& thisEntity = myEntityComponentSystem->GetEntity(thisRewindComponentID);


				//auto& thisEntityTransformComponent = thisEntity->GetComponent<TransformComponent>();

				ECS::TransformComponent* thisEntityTransformComponent = thisEntity->GetComponent<TransformComponent>();

				ECS::RewindTestComponent* thisRewindComponent = thisEntity->GetComponent<RewindTestComponent>();


				GoToPoint(thisEntityTransformComponent, thisRewindComponent);

				RewindDataToRightComponent structData;

				structData.ThisBoolCheck = thisRewindComponent->goToFirstPoint;
				structData.transform = thisEntityTransformComponent->transform;
				
				rewindMap.emplace(thisEntity->GetID(), structData);
			}
			rewindTimeQue.push_back(rewindMap);

			if (rewindTimeQue.size() > 600)
			{
				rewindTimeQue.erase(rewindTimeQue.begin() + 0);
			}


		}

	}
}