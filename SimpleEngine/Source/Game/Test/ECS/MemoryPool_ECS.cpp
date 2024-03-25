#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPool_ECS.hpp"

namespace Simple
{
	MemoryPool_ECS::MemoryPool_ECS(size_t aDefaultSize)
	{
		myStartMemoryAdress = new char[aDefaultSize];
		myEndMemoryAdress = myStartMemoryAdress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAdress = myStartMemoryAdress;

		myElementIDs.reserve(aDefaultSize);
	}

	MemoryPool_ECS::~MemoryPool_ECS()
	{
		delete[] myStartMemoryAdress;
	}

	size_t MemoryPool_ECS::GetSize() const
	{
		return myCurrentMemoryAdress - myStartMemoryAdress;
	}

	size_t MemoryPool_ECS::GetElementIDByIndex(const size_t aIndex) const
	{
		return myElementIDs[aIndex];
	}

	int MemoryPool_ECS::GetElementIndexByAdress(const char* aAdress, const size_t aSize) const
	{
		if (aSize == 0)
		{
			return -1;
		}

		const int index = static_cast<int>((aAdress - myStartMemoryAdress)) / static_cast<int>(aSize);

		if (index < 0)
		{
			return -1;
		}

		return index;
	}

	int MemoryPool_ECS::GetElementIDByMemoryAdress(const char* aAdress) const
	{
		const size_t memorySize = GetSize();
		const size_t elementSize = myElementIDs.size();

		if (memorySize == 0 || elementSize == 0)
		{
			return -1;
		}

		const size_t a = (aAdress - myStartMemoryAdress); a;
		const size_t index = (aAdress - myStartMemoryAdress) / (memorySize / elementSize);

		if (index > myElementIDs.size())
		{
			return -1;
		}

		return static_cast<int>(myElementIDs[index]);
	}

	char* MemoryPool_ECS::GetStartMemoryAdress()
	{
		return myStartMemoryAdress;
	}

	const char* MemoryPool_ECS::GetEndMemoryAdress()
	{
		return myEndMemoryAdress;
	}

	const char* MemoryPool_ECS::GetCurrentMemoryAdress()
	{
		return myCurrentMemoryAdress;
	}

	size_t MemoryPool_ECS::GetElementCount() const
	{
		return myElementIDs.size();
	}

	size_t MemoryPool_ECS::GetCapacity() const
	{
		return myEndMemoryAdress - myStartMemoryAdress;
	}

	size_t MemoryPool_ECS::GetAvaliableMemorySize() const
	{
		return myEndMemoryAdress - myCurrentMemoryAdress;
	}

	void MemoryPool_ECS::Reallocate()
	{
		char* oldMemoryArray = myStartMemoryAdress;

		const size_t oldMemoryCapacity = GetCapacity();
		const size_t currentMemorySize = GetSize();
		const size_t newMemoryCapacity = oldMemoryCapacity * 2;

		myStartMemoryAdress = new char[newMemoryCapacity];
		std::memcpy(myStartMemoryAdress, oldMemoryArray, oldMemoryCapacity);
		delete[] oldMemoryArray;

		myCurrentMemoryAdress = myStartMemoryAdress + currentMemorySize;
		myEndMemoryAdress = myStartMemoryAdress + newMemoryCapacity;
	}
}