#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPools/MemoryPoolECSEntity.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	MemoryPoolECSEntity::MemoryPoolECSEntity(size_t aDefaultSize)
		: myCurrentID(0)
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;
	}

	MemoryPoolECSEntity::~MemoryPoolECSEntity()
	{
		delete[] myStartMemoryAddress;
	}

	Entity& MemoryPoolECSEntity::AllocateEntity()
	{
		constexpr size_t objectSize = sizeof(Entity);
		bool reallocated = false;

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
			reallocated = true;
		}

		new(myCurrentMemoryAddress)Entity(myCurrentID);
		myCurrentMemoryAddress += objectSize;

		myEntityIDs.push_back(myCurrentID);
		myCurrentID++;

		return (Entity&)*(myCurrentMemoryAddress - objectSize);
	}

	Entity& MemoryPoolECSEntity::GetEntityByIndex(const size_t aIndex)
	{
		return (Entity&)*(myStartMemoryAddress + aIndex * sizeof(Entity));
	}

	Entity& MemoryPoolECSEntity::GetEntityByMemoryAddress(const char* aAddress)
	{
		return (Entity&)*(aAddress);
	}

	char* MemoryPoolECSEntity::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	const char* MemoryPoolECSEntity::GetEndMemoryAddress()
	{
		return myEndMemoryAddress;
	}

	const char* MemoryPoolECSEntity::GetCurrentMemoryAddress()
	{
		return myCurrentMemoryAddress;
	}

	std::vector<size_t> MemoryPoolECSEntity::GetEntityIDs() const
	{
		return myEntityIDs;
	}

	std::vector<Entity*> MemoryPoolECSEntity::GetAllEntities()
	{
		const size_t count = myEntityIDs.size();

		if (count == 0)
		{
			return {};
		}

		std::vector<Entity*> entities(count);

		for (size_t i = 0; i < count; ++i)
		{
			entities[i] = reinterpret_cast<Entity*>(myStartMemoryAddress + i * sizeof(Entity));
		}

		return entities;
	}

	const std::vector<Entity*> MemoryPoolECSEntity::GetAllEntities() const
	{
		const size_t count = myEntityIDs.size();

		if (count == 0)
		{
			return {};
		}

		std::vector<Entity*> entities(count);

		for (size_t i = 0; i < count; ++i)
		{
			entities[i] = reinterpret_cast<Entity*>(myStartMemoryAddress + i * sizeof(Entity));
		}

		return entities;
	}

	size_t MemoryPoolECSEntity::GetSize() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECSEntity::GetEntityCount() const
	{
		return myEntityIDs.size();
	}

	size_t MemoryPoolECSEntity::GetEntityIDByIndex(const size_t aIndex) const
	{
		return myEntityIDs[aIndex];
	}

	int MemoryPoolECSEntity::GetEntityIndexByMemoryAddress(const char* aAddress) const
	{
		const int index = static_cast<int>((aAddress - myStartMemoryAddress)) / static_cast<int>(sizeof(Entity));

		if (index < 0)
		{
			return -1;
		}

		return index;
	}

	int MemoryPoolECSEntity::GetEntityIDByMemoryAddress(const char* aAddress) const
	{
		const size_t memorySize = GetSize();
		const size_t elementSize = myEntityIDs.size();

		if (memorySize == 0 || elementSize == 0)
		{
			return -1;
		}
		
		const size_t index = (aAddress - myStartMemoryAddress) / (memorySize / elementSize);

		if (index > myEntityIDs.size())
		{
			return -1;
		}

		return static_cast<int>(myEntityIDs[index]);
	}

	void MemoryPoolECSEntity::Reallocate()
	{
		char* oldMemoryArray = myStartMemoryAddress;

		const size_t oldMemoryCapacity = GetCapacity();
		const size_t currentMemorySize = GetSize();
		const size_t newMemoryCapacity = oldMemoryCapacity * 2;

		myStartMemoryAddress = new char[newMemoryCapacity];
		std::memcpy(myStartMemoryAddress, oldMemoryArray, oldMemoryCapacity);
		delete[] oldMemoryArray;

		myCurrentMemoryAddress = myStartMemoryAddress + currentMemorySize;
		myEndMemoryAddress = myStartMemoryAddress + newMemoryCapacity;
	}

	size_t MemoryPoolECSEntity::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECSEntity::GetAvailableMemorySize() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}
}