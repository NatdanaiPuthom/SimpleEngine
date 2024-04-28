#pragma once
#include <vector>

namespace Simple
{
	class Entity;
	class EntityManager;
}

namespace Simple
{
	class EntityPool final
	{
		using EntityID = size_t;
	public:
		EntityPool();
		~EntityPool();

		EntityPool(const EntityPool&) = delete;
		EntityPool(EntityPool&&) = delete;
		EntityPool& operator=(const EntityPool&) = delete;
		EntityPool& operator=(EntityPool&&) = delete;

		void Init(const size_t aEntityAmountToReserved);

		char* CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityManager* aEntityManager);

		size_t GetCapacity() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvaliableMemorySpace() const;
		size_t GetEntityCount() const;
		char* GetStartMemoryAddress();
		char*& GetStartMemoryAddressRef();
		char*& GetCurrentMemoryAddressRef();

	private:
		void Reallocate();
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		std::vector<size_t> myEntityIDs;
		const char padding[8];
	};
}