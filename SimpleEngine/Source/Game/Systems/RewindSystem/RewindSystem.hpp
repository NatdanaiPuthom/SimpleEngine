#pragma once
#include "Engine/ECS/Core/System.hpp"

#include <unordered_map>
#include <queue>
#include <vector>

namespace ECS
{
	struct TransformComponent;
	struct RewindTestComponent;

	struct RewindDataToRightComponent
	{
		ECS::EntityID entityID;
		Math::Transform transform;
		bool ThisBoolCheck;
	};

	class RewindSystem final : public ECS::System
	{
	public:
		explicit RewindSystem(EntityComponentSystem* aEntityComponentSystem);
		~RewindSystem() override;

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;

		void Init() override;
		void Update() override;

		void GoToPoint(ECS::TransformComponent* aTransformComponent, ECS::RewindTestComponent* aRewindTestComponent);

		std::vector<std::unordered_map<ECS::EntityID, RewindDataToRightComponent>>rewindTimeQue;

		//std::vector<RewindDataToRightComponent> rewindTimeQue;
		//std::queue<RewindDataToRightComponent> rewindTimeQue;
	};
}