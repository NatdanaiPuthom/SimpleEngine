#pragma once
#include "Game/Test/ECS/Core/Entity.hpp"
#include "Game/Test/ECS/MemoryPools/MemoryPoolECSEntity.hpp"
#include <vector>
#include <unordered_map>

namespace Simple
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		Entity& CreateEntity();
		Entity* GetEntity(const size_t aEntityID) const;
		size_t GetEntityID(Entity* aEntity);

		std::vector<Entity*> GetAllEntities();
		const std::vector<Entity*> GetAllEntities() const;

	private:

	private:
		MemoryPoolECSEntity myEntityPool;
		std::unordered_map<size_t, Entity*> myEntityMapToPointer;
		std::unordered_map<Entity*, size_t> myEntityMapToID;
	};
}