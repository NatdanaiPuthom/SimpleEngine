#pragma once
#include <vector>

namespace Simple
{
	class EntityE;
	class EntityM;
}

namespace Simple
{
	class EntityP final
	{
		using EntityID = size_t;
	public:
		EntityP(const size_t aDefaultSize = 64);
		~EntityP();

		EntityP(const EntityP&) = delete;
		EntityP(EntityP&&) = delete;
		EntityP& operator=(const EntityP&) = delete;
		EntityP& operator=(EntityP&&) = delete;

		char* CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityM* aEntityManager);

		size_t GetCapacity() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvaliableMemorySpace() const;
		size_t GetEntityCount() const;
		char* GetStartMemoryAddress();

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