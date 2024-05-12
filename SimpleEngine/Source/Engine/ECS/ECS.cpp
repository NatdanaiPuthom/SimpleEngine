#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace ECS
{
	EntityComponentSystem::EntityComponentSystem()
		: myEntityManager(&myComponentManager)
	{
	}

	EntityComponentSystem::~EntityComponentSystem()
	{
	}

	void EntityComponentSystem::Init()
	{
		constexpr size_t entitiesToReserve = 8; //NOTE(v9.30.10):Small number for experimental purposes for now
		myEntityManager.Init(entitiesToReserve);

		mySystemManager.AddSystem(std::move(std::make_unique<RenderSystem>(&myEntityManager)));

		mySystemManager.Init();
	}

	void EntityComponentSystem::Update()
	{
		mySystemManager.Update();
	}

	void EntityComponentSystem::Render()
	{
		mySystemManager.Render();
	}

	void EntityComponentSystem::AddSystem(std::unique_ptr<System> aSystem)
	{
		mySystemManager.AddSystem(std::move(aSystem));
	}

	Entity EntityComponentSystem::CreateEntity()
	{
		return myEntityManager.CreateEntity();
	}

	Entity EntityComponentSystem::GetEntity(const EntityID aID)
	{
		return myEntityManager.GetEntity(aID);
	}

	Entities EntityComponentSystem::GetAllEntities()
	{
		return myEntityManager.GetAllEntities();
	}

	void EntityComponentSystem::SetGlobalPointerToThis()
	{
		Impl::SimpleWorldECS::SetECS(this);
	}
}