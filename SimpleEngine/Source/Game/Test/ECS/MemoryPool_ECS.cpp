#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/MemoryPool_ECS.hpp"

namespace ECS
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

	size_t MemoryPool_ECS::GetElementIDByMemoryAdress(const char* aAdress) const
	{
		const size_t index = (aAdress - myStartMemoryAdress) / (GetSize() / myElementIDs.size());
		return myElementIDs[index];
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