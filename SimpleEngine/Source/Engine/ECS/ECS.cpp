#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace Simple
{
	ECS::ECS()
		: myEntityManager(&myComponentManager)
	{
	}

	ECS::~ECS()
	{
	}

	void ECS::Init()
	{
		constexpr size_t entitiesToReserve = 8; //NOTE(v9.30.10):Small number for experimental purposes for now
		myEntityManager.Init(entitiesToReserve);
	}

	Entity ECS::CreateEntity()
	{
		return myEntityManager.CreateEntity();
	}

	Entity ECS::GetEntity(const EntityID aID)
	{
		return myEntityManager.GetEntity(aID);
	}

	Entities ECS::GetAllEntities()
	{
		return myEntityManager.GetAllEntities();
	}

	void ECS::SetGlobalPointerToThis()
	{
		Impl::SimpleWorldECS::SetECS(this);
	}
}