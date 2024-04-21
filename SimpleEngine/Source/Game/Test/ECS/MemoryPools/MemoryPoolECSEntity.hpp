#pragma once
#include <vector>

//TO-DO(v9.28.8): removing entities and more tests
//NOTES(v9.28.8): should we 

namespace Simple
{
	class Entity;
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

		Entity& AllocateEntity();

		Entity& GetEntityByIndex(const size_t aIndex);
		Entity& GetEntityByMemoryAddress(const char* aAddress);

		size_t GetSize() const;
		size_t GetEntityCount() const;
		size_t GetEntityIDByIndex(const size_t aIndex) const;

		int GetEntityIndexByMemoryAddress(const char* aAdress) const;
		int GetEntityIDByMemoryAddress(const char* aAddress) const;

		std::vector<size_t> GetEntityIDs() const;
		std::vector<Entity*> GetAllEntities();
		const std::vector<Entity*> GetAllEntities() const;

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