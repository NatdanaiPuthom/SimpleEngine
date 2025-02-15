#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	struct AbilitComponent;

	class AbilitySystem final : public ECS::System
	{
	public:
		AbilitySystem();
		~AbilitySystem() override;

		void Init(EntityComponentSystem* aEntityComponentSystem) override;
		void Update(EntityComponentSystem* aEntityComponentSystem) override;
		void Render(EntityComponentSystem* aEntityComponentSystem) override;

		void EarlyUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void FixedUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void LateUpdate(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	public:
		std::unordered_map<EntityID, std::vector<AbilitComponent>> EntitysAbilities;

		AbilitComponent CreatAbility(std::string aName, int aManaCost, int aKey/*, EntityID aEntityID*/, void (*aExecute)(void));

		bool oneTime = true;
	};
}