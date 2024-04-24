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

		Entity CreateEntity();
		EntityClass* GetEntity(const size_t aEntityID) const;

		std::vector<EntityClass*> GetAllEntities();
		const std::vector<EntityClass*> GetAllEntities() const;

	private:
		MemoryPoolECSEntity myEntityPool;
		std::unordered_map<size_t, EntityClass*> myEntityMapToPointer;
	};
}