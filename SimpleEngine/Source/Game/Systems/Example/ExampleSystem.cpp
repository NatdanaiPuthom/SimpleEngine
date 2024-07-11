#include "Game/Precomplied/GamePch.hpp"
#include "Game/Systems/Example/ExampleSystem.hpp"

namespace ECS
{
	ExampleSystem::ExampleSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	ExampleSystem::~ExampleSystem()
	{
	}

	void ExampleSystem::Init()
	{
		std::cout << "Initialized ExampleSystem!" << std::endl;
	}

	void ExampleSystem::Update()
	{
		std::cout << "ExampleSystem is begin updated!" << std::endl;
	}

	void ExampleSystem::Render()
	{
	}

	void ExampleSystem::EarlyUpdate()
	{
	}

	void ExampleSystem::FixedUpdate()
	{
	}

	void ExampleSystem::LateUpdate()
	{
	}

	std::unique_ptr<System> ExampleSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<ExampleSystem>(aEntityComponentSystem);
	}
}