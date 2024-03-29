#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/ECS.hpp"

namespace Simple
{
	ECS::ECS()
	{
	}

	ECS::~ECS()
	{
	}

	void ECS::Init()
	{
		myComponentManager.Init();
		mySystemManager.Init();
	}

	void ECS::Update()
	{
		mySystemManager.Update();
	}

	void ECS::Render()
	{
		mySystemManager.Render();
	}

	Entity& ECS::CreateEntity()
	{
		return myEntityManager.CreateEntity();
	}

	size_t ECS::GetEntityID(Entity* aEntity)
	{
		return myEntityManager.GetEntityID(aEntity);
	}

	Entity* ECS::GetEntity(const size_t aEntityID) const
	{
		return myEntityManager.GetEntity(aEntityID);
	}

	std::vector<Entity>& ECS::GetAllEntities()
	{
		return myEntityManager.GetAllEntities();
	}

	const std::vector<Entity>& ECS::GetAllEntities() const
	{
		return myEntityManager.GetAllEntities();
	}
}