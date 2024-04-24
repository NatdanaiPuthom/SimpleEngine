#pragma once
#include <vector>
#include <unordered_map>

//TO-DO(v9.28.8): removing entities and more tests
//NOTES(v9.28.8): should we return std::vector<Entity*> or std::vector<Entity*>& ?

namespace Simple
{
	class EntityClass;
}

namespace Simple
{
	class MemoryPoolECSEntity final
	{
	public:
		MemoryPoolECSEntity(size_t aDefaultSize = 128);
		~MemoryPoolECSEntity();

		MemoryPoolECSEntity(const MemoryPoolECSEntity&) = delete;
		MemoryPoolECSEntity(MemoryPoolECSEntity&&) = delete;
		MemoryPoolECSEntity& operator=(const MemoryPoolECSEntity&) = delete;
		MemoryPoolECSEntity& operator=(MemoryPoolECSEntity&&) = delete;

		EntityClass& AllocateEntity(std::unordered_map<size_t, EntityClass*>& aEntityClassMap);

		EntityClass& GetEntityByIndex(const size_t aIndex);
		EntityClass& GetEntityByMemoryAddress(const char* aAddress);

		size_t GetSize() const;
		size_t GetEntityCount() const;
		size_t GetEntityIDByIndex(const size_t aIndex) const;

		int GetEntityIndexByMemoryAddress(const char* aAdress) const;
		int GetEntityIDByMemoryAddress(const char* aAddress) const;

		std::vector<size_t> GetEntityIDs() const;
		std::vector<EntityClass*> GetAllEntities();
		const std::vector<EntityClass*> GetAllEntities() const;

		char* GetStartMemoryAddress();
		const char* GetEndMemoryAddress();
		const char* GetCurrentMemoryAddress();

	private:
		void Reallocate();
		size_t GetCapacity() const;
		size_t GetAvailableMemorySize() const;

	private:
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myCurrentMemoryAddress;

		size_t myCurrentID;
		std::vector<size_t> myEntityIDs;
	};
}