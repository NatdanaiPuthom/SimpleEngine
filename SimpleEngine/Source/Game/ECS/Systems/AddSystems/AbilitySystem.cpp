#include "Game/Precomplied/GamePch.hpp"
#include "Game/ECS/Systems/AddSystems/AbilitySystem.hpp"
#include "Game/ECS/Components/AddComponents/AbilityComponent.hpp"

namespace ECS
{
	AbilitySystem::AbilitySystem()
	{
	}
	AbilitySystem::~AbilitySystem()
	{
	}
	void AbilitySystem::Init(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void AbilitySystem::Update(EntityComponentSystem* /*aEntityComponentSystem*/)
	{

	}
	void AbilitySystem::Render(EntityComponentSystem* /*aEntityComponentSystem*/)
	{

	}
	void AbilitySystem::EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void AbilitySystem::FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	void AbilitySystem::LateUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
	}
	std::unique_ptr<System> AbilitySystem::Clone() const
	{
		return std::make_unique<AbilitySystem>(*this);
	}
}

