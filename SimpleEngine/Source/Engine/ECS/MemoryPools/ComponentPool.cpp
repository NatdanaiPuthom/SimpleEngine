#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <cassert>
#include <algorithm>

namespace ECS
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

	void ComponentPool::Remap(const std::vector<ComponentID>& aComponentIDs, const size_t aSize)
	{
		myIDToPointer = std::unordered_map<ComponentID, char*>();
		myPointerToID = std::unordered_map<char*, ComponentID>();

		for (size_t i = 0; i < aComponentIDs.size(); ++i)
		{
			char* newAddress = myStartMemoryAddress + i * aSize;
			myIDToPointer[aComponentIDs[i]] = newAddress;
			myPointerToID[newAddress] = aComponentIDs[i];
		}
	}

	std::vector<ComponentID> ComponentPool::SortMemoryAddressesAndReturnSortedComponentIDs()
	{
		if (myPointerToID.size() <= 0)
		{
			return std::vector<ComponentID>();
		}

		std::vector<char*> oldPointerAddresses;
		std::vector<ComponentID> oldComponentIDs;

		oldComponentIDs.reserve(myPointerToID.size());
		oldPointerAddresses.reserve(myIDToPointer.size());

		for (auto& [id, pointer] : myIDToPointer)
		{
			oldPointerAddresses.push_back(pointer);
		}

		bool swapped;

		do
		{
			swapped = false;

			for (int i = 0; i < oldPointerAddresses.size() - 1; ++i)
			{
				if (oldPointerAddresses[i] > oldPointerAddresses[static_cast<size_t>(i) + 1])
				{
					std::swap(oldPointerAddresses[i], oldPointerAddresses[static_cast<size_t>(i) + 1]);
					swapped = true;
				}
			}
		} while (swapped);

		for (auto& pointer : oldPointerAddresses)
		{
			oldComponentIDs.push_back(myPointerToID[pointer]);
		}

		return oldComponentIDs;
	}

	bool ComponentPool::SwapWithLastAndRemoveEditor(const size_t aComponentID, const std::type_index& aTypeIndex)
	{
		MainSingleton::GetComponentRegistry()->myTypeErasureComponentDestructorInvoker[aTypeIndex](static_cast<void*>(myIDToPointer[aComponentID]));

		if (myIDToPointer[aComponentID] == (myCurrentMemoryAddress - myComponentTypeSize))
		{
			//NOTE(v11.3.0): Should refactor in future as the code is duplicate

			memset(myCurrentMemoryAddress - myComponentTypeSize, '\0', myComponentTypeSize);
			myPointerToID.erase(myIDToPointer[aComponentID]);
			myIDToPointer.erase(aComponentID);
			return true;
		}

		myCurrentMemoryAddress -= myComponentTypeSize;

		//std::memcpy(myIDToPointer[aComponentID], myCurrentMemoryAddress, myComponentTypeSize); //NOTE(v11.3.0): Fuck you memcpy, I couldn't slept for 2 days because of you
		std::swap(myIDToPointer[aComponentID], myCurrentMemoryAddress);
		memset(myCurrentMemoryAddress, '\0', myComponentTypeSize);

		myPointerToID.erase(myIDToPointer[aComponentID]);
		myIDToPointer.erase(aComponentID);

		return true;
	}

	size_t ComponentPool::GetCapacity() const
	{
		return myEndMemoryAddress - myStartMemoryAddress;
	}

	size_t ComponentPool::GetComponentCount() const
	{
		return myIDToPointer.size();
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

	size_t ComponentPool::GetComponentIndexByMemoryAddress(char* aAddress) const
	{
		return static_cast<int>((aAddress - myStartMemoryAddress)) / static_cast<int>(myComponentTypeSize);
	}

	std::unordered_map<size_t, char*>& ComponentPool::GetComponentIDToPointerMap()
	{
		return myIDToPointer;
	}

	char* ComponentPool::GetStartMemoryAddress()
	{
		return myStartMemoryAddress;
	}

	char* ComponentPool::GetComponentAddressByID(const size_t aComponentID)
	{
		if (myIDToPointer.contains(aComponentID))
		{
			return myIDToPointer[aComponentID];
		}

		assert(false && "ComponentID does not exist");
		return nullptr;
	}
}