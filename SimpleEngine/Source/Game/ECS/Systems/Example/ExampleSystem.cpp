#include "Game/Precomplied/GamePch.hpp"
#include "Game/ECS/Systems/Example/ExampleSystem.hpp"

namespace ECS
{
	ExampleSystem::ExampleSystem()
	{
	}

	ExampleSystem::~ExampleSystem()
	{
	}

	void ExampleSystem::Init(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "Initialized ExampleSystem!" << std::endl;
	}

	void ExampleSystem::Update(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "ExampleSystem is begin updated!" << std::endl;
	}

	void ExampleSystem::Render(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "ExampleSystem is begin rendered!" << std::endl;
	}

	void ExampleSystem::EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "ExampleSystem is begin early updated!" << std::endl;
	}

	void ExampleSystem::FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "ExampleSystem is begin fixed updated!" << std::endl;
	}

	void ExampleSystem::LateUpdate(EntityComponentSystem* /*aEntityComponentSystem*/)
	{
		std::cout << "ExampleSystem is begin late updated!" << std::endl;
	}

	std::unique_ptr<System> ExampleSystem::Clone() const
	{
		return std::make_unique<ExampleSystem>();
	}
}