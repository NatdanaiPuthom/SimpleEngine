#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPool_ECS.hpp"

namespace Simple
{
	MemoryPool_ECS::MemoryPool_ECS(size_t aDefaultSize)
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;

		myElementIDs.reserve(aDefaultSize);
	}

	MemoryPool_ECS::~MemoryPool_ECS()
	{
		delete[] myStartMemoryAddress;
	}

	size_t MemoryPool_ECS::GetSize() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPool_ECS::GetElementIDByIndex(const size_t aIndex) const
	{
		return myElementIDs[aIndex];
	}

	int MemoryPool_ECS::GetElementIndexByMemoryAddress(const char* aAdress, const size_t aSize) const
	{
		if (aSize == 0)
		{
			return -1;
		}

		const int index = static_cast<int>((aAdress - myStartMemoryAddress)) / static_cast<int>(aSize);

		if (index < 0)
		{
			return -1;
		}

		return index;
	}

	int MemoryPool_ECS::GetElementIDByMemoryAddress(const char* aAdress) const
	{
		const size_t memorySize = GetSize();
		const size_t elementSize = myElementIDs.size();

		if (memorySize == 0 || elementSize == 0)
		{
			return -1;
		}

		const size_t a = (aAdress - myStartMemoryAddress); a;
		const size_t index = (aAdress - myStartMemoryAddress) / (memorySize / elementSize);

		if (index > myElementIDs.size())
		{
			return -1;
		}

		return static_cast<int>(myElementIDs[index]);
	}

	char* MemoryPool_ECS::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	const char* MemoryPool_ECS::GetEndMemoryAddress()
	{
		return myEndMemoryAddress;
	}

	const char* MemoryPool_ECS::GetCurrentMemoryAddress()
	{
		return myCurrentMemoryAddress;
	}

	size_t MemoryPool_ECS::GetElementCount() const
	{
		return myElementIDs.size();
	}

	size_t MemoryPool_ECS::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPool_ECS::GetAvailableMemorySize() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	void MemoryPool_ECS::Reallocate()
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
}