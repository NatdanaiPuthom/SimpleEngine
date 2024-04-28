#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/EntityP.hpp"
#include "Game/Test/ECS2/EntityE.hpp"

namespace Simple
{
	EntityP::EntityP(const size_t aDefaultSize)
		: padding("Believe")
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;
	}

	EntityP::~EntityP()
	{
		delete[] myStartMemoryAddress;

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}

	char* EntityP::CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityM* aEntityManager)
	{
		bool shouldMoveEntitiesToNewAddress = false;

		while (sizeof(EntityE) > GetAvaliableMemorySpace())
		{
			Reallocate();
			shouldMoveEntitiesToNewAddress = true;
		}

		if (shouldMoveEntitiesToNewAddress == true)
		{
			for (size_t i = 0; i < GetEntityCount(); ++i)
			{
				aEntities[myEntityIDs[i]] = myStartMemoryAddress + i * sizeof(EntityE);
			}
		}

		new(myCurrentMemoryAddress)EntityE(aID, aEntityManager);
		myCurrentMemoryAddress += sizeof(EntityE);
		myEntityIDs.push_back(aID);

		return myCurrentMemoryAddress - sizeof(EntityE);
	}

	size_t EntityP::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t EntityP::GetOccupiedMemorySpace() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t EntityP::GetAvaliableMemorySpace() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	size_t EntityP::GetEntityCount() const
	{
		return myEntityIDs.size();
	}

	char* EntityP::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	void EntityP::Reallocate()
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