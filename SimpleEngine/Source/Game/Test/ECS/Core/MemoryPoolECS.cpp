#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/MemoryPoolECS.hpp"

namespace Simple
{
	MemoryPoolECS::MemoryPoolECS(size_t aDefaultSize)
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;

		myElementIDs.reserve(aDefaultSize);
	}

	MemoryPoolECS::~MemoryPoolECS()
	{
		delete[] myStartMemoryAddress;
	}

	size_t MemoryPoolECS::GetSize() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECS::GetElementIDByIndex(const size_t aIndex) const
	{
		return myElementIDs[aIndex];
	}

	int MemoryPoolECS::GetElementIndexByMemoryAddress(const char* aAdress, const size_t aSize) const
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

	int MemoryPoolECS::GetElementIDByMemoryAddress(const char* aAdress) const
	{
		const size_t memorySize = GetSize();
		const size_t elementSize = myElementIDs.size();

		if (memorySize == 0 || elementSize == 0)
		{
			return -1;
		}

		const size_t index = (aAdress - myStartMemoryAddress) / (memorySize / elementSize);

		if (index > myElementIDs.size())
		{
			return -1;
		}

		return static_cast<int>(myElementIDs[index]);
	}

	std::vector<size_t> MemoryPoolECS::GetElementIDs() const
	{
		return myElementIDs;
	}

	char* MemoryPoolECS::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	const char* MemoryPoolECS::GetEndMemoryAddress()
	{
		return myEndMemoryAddress;
	}

	const char* MemoryPoolECS::GetCurrentMemoryAddress()
	{
		return myCurrentMemoryAddress;
	}

	size_t MemoryPoolECS::GetElementCount() const
	{
		return myElementIDs.size();
	}

	size_t MemoryPoolECS::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECS::GetAvailableMemorySize() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	void MemoryPoolECS::Reallocate()
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