#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/EntityManager.hpp"

namespace Simple
{
	EntityManager::EntityManager()
	{
	}

	EntityManager::~EntityManager()
	{
		const std::size_t count = myEntityPool.GetEntityCount();
		char* entities = myEntityPool.GetStartMemoryAddress();

		for (size_t i = 0; i < count; ++i)
		{
			const std::size_t offset = i * sizeof(EntityClass);
			myEntityPool.GetEntityByMemoryAddress(entities + offset).~EntityClass();
		}
	}

	Entity EntityManager::CreateEntity()
	{
		Simple::EntityClass& entity = myEntityPool.AllocateEntity();
		const size_t id = entity.GetID();

		myEntityMapToPointer[id] = &entity;
		myEntityMapToID[&entity] = id;

		return myEntityMapToPointer[id];
	}

	EntityClass* EntityManager::GetEntity(const size_t aEntityID) const
	{
		auto it = myEntityMapToPointer.find(aEntityID);

		if (it != myEntityMapToPointer.end())
		{
			return it->second;
		}

		return nullptr;
	}

	size_t EntityManager::GetEntityID(EntityClass* aEntity)
	{
		if (aEntity == nullptr)
		{
			assert(false && "Could not find Entity");
			return static_cast<size_t>(-1);
		}

		auto it = myEntityMapToID.find(aEntity);

		if (it != myEntityMapToID.end())
		{
			return myEntityMapToID[aEntity];
		}
		else
		{
			assert(false && "Could not find Entity");
			return static_cast<size_t>(-1);
		}
	}

	const std::vector<EntityClass*> EntityManager::GetAllEntities() const
	{
		return myEntityPool.GetAllEntities();
	}

	std::vector<EntityClass*> EntityManager::GetAllEntities()
	{
		return myEntityPool.GetAllEntities();
	}
}