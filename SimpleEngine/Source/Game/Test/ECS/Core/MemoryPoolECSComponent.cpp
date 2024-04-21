#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/MemoryPoolECSComponent.hpp"

namespace Simple
{
	MemoryPoolECSComponent::MemoryPoolECSComponent(size_t aDefaultSize)
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;

		myElementIDs.reserve(aDefaultSize);
	}

	MemoryPoolECSComponent::~MemoryPoolECSComponent()
	{
		delete[] myStartMemoryAddress;
	}

	size_t MemoryPoolECSComponent::GetSize() const
	{
		return myCurrentMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECSComponent::GetElementIDByIndex(const size_t aIndex) const
	{
		return myElementIDs[aIndex];
	}

	int MemoryPoolECSComponent::GetElementIndexByMemoryAddress(const char* aAdress, const size_t aSize) const
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

	int MemoryPoolECSComponent::GetElementIDByMemoryAddress(const char* aAdress) const
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

	std::vector<size_t> MemoryPoolECSComponent::GetElementIDs() const
	{
		return myElementIDs;
	}

	char* MemoryPoolECSComponent::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	const char* MemoryPoolECSComponent::GetEndMemoryAddress()
	{
		return myEndMemoryAddress;
	}

	const char* MemoryPoolECSComponent::GetCurrentMemoryAddress()
	{
		return myCurrentMemoryAddress;
	}

	size_t MemoryPoolECSComponent::GetElementCount() const
	{
		return myElementIDs.size();
	}

	size_t MemoryPoolECSComponent::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t MemoryPoolECSComponent::GetAvailableMemorySize() const
	{
		return myEndMemoryAddress - myCurrentMemoryAddress;
	}

	void MemoryPoolECSComponent::Reallocate()
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