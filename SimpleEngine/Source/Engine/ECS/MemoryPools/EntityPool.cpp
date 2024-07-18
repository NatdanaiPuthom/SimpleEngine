#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include "Engine/ECS/Core/Entity.hpp"

namespace ECS
{
	EntityPool::EntityPool()
		: myStartMemoryAddress(nullptr)
		, myEndMemoryAddress(nullptr)
		, myCurrentMemoryAddress(nullptr)
		, myPadding("Believ\0")
	{
		Init(1);
	}

	EntityPool::~EntityPool()
	{
		Clear();
	}

	EntityPool::EntityPool(const EntityPool& aOther)
	{
		const size_t size = aOther.myEndMemoryAddress - aOther.myStartMemoryAddress;
		const size_t offset = aOther.myCurrentMemoryAddress - aOther.myStartMemoryAddress;

		this->myStartMemoryAddress = new char[size];
		this->myEndMemoryAddress = this->myStartMemoryAddress + sizeof(char) * size;
		this->myCurrentMemoryAddress = this->myStartMemoryAddress + offset;

		memset(this->myCurrentMemoryAddress, '\0', this->myEndMemoryAddress - this->myCurrentMemoryAddress);
		memcpy(this->myPadding, aOther.myPadding, sizeof(this->myPadding));

		this->myEntityIDs = aOther.myEntityIDs;
	}

	void EntityPool::Init(const size_t aEntityAmountToReserved)
	{
		Clear();

		const size_t size = aEntityAmountToReserved * sizeof(IEntity);
		myStartMemoryAddress = new char[size];
		myEndMemoryAddress = myStartMemoryAddress + size;
		myCurrentMemoryAddress = myStartMemoryAddress;

		memset(myStartMemoryAddress, '\0', size);
	}

	char* EntityPool::CreateEntity(const EntityID aID, std::unordered_map<EntityID, char*>& aEntities, EntityManager* aEntityManager)
	{
		bool shouldMoveEntitiesToNewAddress = false;

		while (sizeof(IEntity) > GetAvaliableMemorySpace())
		{
			Reallocate();
			shouldMoveEntitiesToNewAddress = true;
		}

		if (shouldMoveEntitiesToNewAddress == true)
		{
			for (size_t i = 0; i < GetEntityCount(); ++i)
			{
				aEntities[myEntityIDs[i]] = myStartMemoryAddress + i * sizeof(IEntity);
			}
		}

		new(myCurrentMemoryAddress)IEntity(aID, aEntityManager);
		myCurrentMemoryAddress += sizeof(IEntity);
		myEntityIDs.push_back(aID);

		return myCurrentMemoryAddress - sizeof(IEntity);
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

		memset(myCurrentMemoryAddress, '\0', myEndMemoryAddress - myCurrentMemoryAddress);
	}

	void EntityPool::Clear()
	{
		for (size_t i = 0; i < myEntityIDs.size(); ++i)
		{
			reinterpret_cast<IEntity*>(myStartMemoryAddress + i * sizeof(IEntity))->~IEntity();
		}

		delete[] myStartMemoryAddress;

		memset(myStartMemoryAddress, '\0', myEndMemoryAddress - myStartMemoryAddress);

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}
}