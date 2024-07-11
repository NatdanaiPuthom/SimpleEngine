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
		std::cout << "ExampleSystem is begin rendered!" << std::endl;
	}

	void ExampleSystem::EarlyUpdate()
	{
		std::cout << "ExampleSystem is begin early updated!" << std::endl;
	}

	void ExampleSystem::FixedUpdate()
	{
		std::cout << "ExampleSystem is begin fixed updated!" << std::endl;
	}

	void ExampleSystem::LateUpdate()
	{
		std::cout << "ExampleSystem is begin late updated!" << std::endl;
	}

	std::unique_ptr<System> ExampleSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<ExampleSystem>(aEntityComponentSystem);
	}
}