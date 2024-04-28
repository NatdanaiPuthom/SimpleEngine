#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPools/ComponentPool.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	EntityPool::EntityPool()
		: myStartMemoryAddress(nullptr)
		, myEndMemoryAddress(nullptr)
		, myCurrentMemoryAddress(nullptr)
		, padding("Believe")
	{
	}

	EntityPool::~EntityPool()
	{
		delete[] myStartMemoryAddress;

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}

	void EntityPool::Init(const size_t aEntityAmountToReserved)
	{
		const size_t size = aEntityAmountToReserved * sizeof(Entity);
		myStartMemoryAddress = new char[size];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * size;
		myCurrentMemoryAddress = myStartMemoryAddress;
	}

	char* EntityPool::CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityManager* aEntityManager)
	{
		bool shouldMoveEntitiesToNewAddress = false;

		while (sizeof(Entity) > GetAvaliableMemorySpace())
		{
			Reallocate();
			shouldMoveEntitiesToNewAddress = true;
		}

		if (shouldMoveEntitiesToNewAddress == true)
		{
			for (size_t i = 0; i < GetEntityCount(); ++i)
			{
				aEntities[myEntityIDs[i]] = myStartMemoryAddress + i * sizeof(Entity);
			}
		}

		new(myCurrentMemoryAddress)Entity(aID, aEntityManager);
		myCurrentMemoryAddress += sizeof(Entity);
		myEntityIDs.push_back(aID);

		return myCurrentMemoryAddress - sizeof(Entity);
	}

	size_t EntityPool::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t EntityPool::GetOccupiedMemorySpace() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t EntityPool::GetAvaliableMemorySpace() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	size_t EntityPool::GetEntityCount() const
	{
		return myEntityIDs.size();
	}

	char* EntityPool::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	char*& EntityPool::GetStartMemoryAddressRef()
	{
		return myStartMemoryAddress;
	}

	char*& EntityPool::GetCurrentMemoryAddressRef()
	{
		return myCurrentMemoryAddress;
	}

	void EntityPool::Reallocate()
	{
		char* oldMemoryArray = myStartMemoryAddress;

		const size_t currentOccupiedMemorySpace = GetOccupiedMemorySpace();
		const size_t oldMemoryCapacity = GetCapacity();
		const size_t newMemoryCapacity = oldMemoryCapacity * 2;

		myStartMemoryAddress = new char[newMemoryCapacity];
		std::memcpy(myStartMemoryAddress, oldMemoryArray, currentOccupiedMemorySpace);
		delete[] oldMemoryArray;

		myCurrentMemoryAddress = myStartMemoryAddress + currentOccupiedMemorySpace;
		myEndMemoryAddress = myStartMemoryAddress + newMemoryCapacity;
	}
}