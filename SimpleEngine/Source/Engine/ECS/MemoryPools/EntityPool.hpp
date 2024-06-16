#pragma once
#include <vector>
#include <unordered_map>

namespace ECS
{
	class EntityManager;
}

namespace ECS
{
	class EntityPool final
	{
		using EntityID = size_t;

		friend class EntityManager;
	public:
		void Init(const size_t aEntityAmountToReserved);

		char* CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityManager* aEntityManager);

		size_t GetCapacity() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvaliableMemorySpace() const;
		size_t GetEntityCount() const;
		char* GetStartMemoryAddress();

	private:
		EntityPool();
		~EntityPool();

		void Reallocate();
		void Clear();
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		std::vector<size_t> myEntityIDs;
		char padding[8];
	};
}