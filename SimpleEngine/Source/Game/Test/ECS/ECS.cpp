#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/ECS.hpp"
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
		constexpr size_t entitiesToReserve = 8;
		myEntityManager.Init(entitiesToReserve);
	}

	IEntity*& ECS::CreateEntity()
	{
		return myEntityManager.CreateEntity();
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