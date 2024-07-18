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

		memset(myStartMemoryAddress, '\0', sizeof(char) * aDefaultSize);
	}

	ComponentPool::~ComponentPool()
	{
		delete[] myStartMemoryAddress;

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}

	ComponentPool::ComponentPool(const ComponentPool& aOther)
	{
		const size_t size = aOther.myEndMemoryAddress - aOther.myStartMemoryAddress;
		const size_t offset = aOther.myCurrentMemoryAddress - aOther.myStartMemoryAddress;

		this->myComponentTypeSize = aOther.myComponentTypeSize;

		this->myStartMemoryAddress = new char[size];
		this->myEndMemoryAddress = myStartMemoryAddress + size;
		this->myCurrentMemoryAddress = myStartMemoryAddress + offset;

		memcpy(this->myStartMemoryAddress, aOther.myStartMemoryAddress, aOther.myCurrentMemoryAddress - aOther.myStartMemoryAddress);
		memset(this->myCurrentMemoryAddress, '\0', myEndMemoryAddress - myCurrentMemoryAddress);

		std::vector<ComponentID> sortedComponentIDs = aOther.ReturnComponentIDsSortedByAddress();

		for (size_t i = 0; i < sortedComponentIDs.size(); i++)
		{
			char* componentPointer = myStartMemoryAddress + i * myComponentTypeSize;
			const size_t componentID = sortedComponentIDs[i];

			this->myIDToPointer.emplace(componentID, componentPointer);
			this->myPointerToID.emplace(componentPointer, componentID);
		}
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

		memset(myCurrentMemoryAddress, '\0', myEndMemoryAddress - myCurrentMemoryAddress);
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

	std::vector<ComponentID> ComponentPool::ReturnComponentIDsSortedByAddress() const
	{
		if (myPointerToID.size() <= 0)
		{
			return std::vector<ComponentID>();
		}

		std::vector<char*> oldPointerAddresses;
		std::vector<ComponentID> oldComponentIDs;

		oldComponentIDs.reserve(myPointerToID.size());
		oldPointerAddresses.reserve(myIDToPointer.size());

		for (const auto& [id, pointer] : myIDToPointer)
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
			oldComponentIDs.push_back(myPointerToID.at(pointer));
		}

		return oldComponentIDs;
	}

	bool ComponentPool::SwapWithLastComponentAndRemove(const size_t aComponentID, const std::type_index& aTypeIndex)
	{
		//NOTE(v11.3.3): May crash sometime, still havent figure out reason as it was hard to recreate the bug

		if (myCurrentMemoryAddress <= myStartMemoryAddress)
		{
			assert(false && "Invalid removal of component as there are no components allocated.");
			return false;
		}

		char* componentToRemove = myIDToPointer.at(aComponentID);
		myCurrentMemoryAddress -= myComponentTypeSize;

		const size_t lastComponentID = myPointerToID.at(myCurrentMemoryAddress);

		MainSingleton::GetComponentRegistry()->myTypeErasureComponents.at(aTypeIndex.hash_code()).CopyFunctionPointer(componentToRemove, myCurrentMemoryAddress);
		MainSingleton::GetComponentRegistry()->myTypeErasureComponentDestructorInvoker.at(aTypeIndex)(static_cast<void*>(myCurrentMemoryAddress));
		memset(myCurrentMemoryAddress, '\0', myComponentTypeSize);

		myPointerToID.erase(myCurrentMemoryAddress);
		myIDToPointer.erase(aComponentID);

		if (componentToRemove > myStartMemoryAddress)
		{
			myPointerToID[componentToRemove] = lastComponentID;
			myIDToPointer[lastComponentID] = componentToRemove;
		}

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