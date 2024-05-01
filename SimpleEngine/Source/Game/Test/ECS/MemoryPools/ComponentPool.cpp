#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPools/ComponentPool.hpp"

namespace Simple
{
	ComponentPool::ComponentPool(const size_t aDefaultSize)
		: myComponentTypeSize(0)
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;
	}

	ComponentPool::~ComponentPool()
	{
		delete[] myStartMemoryAddress;

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}

	void ComponentPool::Reallocate()
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

	bool ComponentPool::SwapWithLastAndRemoveEditor(const size_t aComponentID)
	{
		const size_t index = GetComponentIndexFromComponentID(aComponentID);
		const int indexToRemove = GetComponentIndexByMemoryAddress(myStartMemoryAddress + index * myComponentTypeSize, myComponentTypeSize);
		const int lastIndex = GetComponentIndexByMemoryAddress(myCurrentMemoryAddress - myComponentTypeSize, myComponentTypeSize);

		if (lastIndex == -1 || indexToRemove == -1)
		{
			return false;
		}

		if (myComponentIDs.size() > 1)
		{
			std::memcpy(myStartMemoryAddress + index * myComponentTypeSize, myCurrentMemoryAddress - myComponentTypeSize, myComponentTypeSize);
			std::swap(myComponentIDs[indexToRemove], myComponentIDs[lastIndex]);
		}

		myCurrentMemoryAddress -= myComponentTypeSize;
		memset(myCurrentMemoryAddress, 0, myComponentTypeSize);
		myComponentIDs.pop_back();

		return true;
	}

	size_t ComponentPool::GetComponentIndexFromComponentID(const size_t aComponentID) const
	{
		size_t index = 0;

		for (size_t i = 0; i < myComponentIDs.size(); ++i)
		{
			if (aComponentID == myComponentIDs[i])
			{
				index = i;
				break;
			}
		}

		return index;
	}

	size_t ComponentPool::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t ComponentPool::GetComponentCount() const
	{
		return myComponentIDs.size();
	}

	size_t ComponentPool::GetComponentTypeSize() const
	{
		return myComponentTypeSize;
	}

	size_t ComponentPool::GetOccupiedMemorySpace() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t ComponentPool::GetAvailableMemorySpace() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	int ComponentPool::GetComponentIndexByMemoryAddress(char* aAddress, const size_t aSize) const
	{
		if (aSize == 0)
		{
			return -1;
		}

		const int index = static_cast<int>((aAddress - myStartMemoryAddress)) / static_cast<int>(aSize);

		if (index < 0)
		{
			return -1;
		}

		return index;
	}

	char* ComponentPool::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	char* ComponentPool::GetComponentAddressByID(const size_t aComponentID)
	{
		const size_t index = GetComponentIndexFromComponentID(aComponentID);
		return myStartMemoryAddress + index * myComponentTypeSize;
	}
}