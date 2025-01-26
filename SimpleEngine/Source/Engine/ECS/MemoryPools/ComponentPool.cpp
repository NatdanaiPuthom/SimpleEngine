#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <cassert>
#include <algorithm>

namespace ECS
{
	ComponentPool::ComponentPool(const size_t aDefaultSize, const std::string& aComponentName)
		: myComponentTypeSize(0)
		, myTypeHashCode(static_cast<size_t>(-1))
	{
		myStartMemoryAddress = new char[aDefaultSize];
		myEndMemoryAddress = myStartMemoryAddress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAddress = myStartMemoryAddress;
		myComponentTypeName = aComponentName;

		memset(myCurrentMemoryAddress, '\0', GetAvailableMemorySpace());
	}

	ComponentPool::~ComponentPool()
	{
		const size_t offset = GetOccupiedMemorySpace();
		size_t componentCount = 0;

		if (myComponentTypeSize != 0)
		{
			componentCount = offset / myComponentTypeSize;
		}

		const ECS::ComponentRegistry* componentRegistry = MainSingleton::GetComponentRegistry();

		for (size_t i = 0; i < componentCount; i++)
		{
			const size_t componentOffset = i * myComponentTypeSize;
			void* sourceAddress = myStartMemoryAddress + componentOffset;
			componentRegistry->DestroyComponent(myTypeHashCode, sourceAddress);
		}

		delete[] myStartMemoryAddress;

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;
	}

	ComponentPool::ComponentPool(const ComponentPool& aOther)
	{
		const size_t size = aOther.GetCapacity();
		const size_t offsetFromStart = aOther.GetOccupiedMemorySpace();

		this->myComponentTypeSize = aOther.myComponentTypeSize;

		this->myStartMemoryAddress = new char[size];
		this->myEndMemoryAddress = this->myStartMemoryAddress + size;
		this->myCurrentMemoryAddress = this->myStartMemoryAddress + offsetFromStart;
		this->myTypeHashCode = aOther.myTypeHashCode;
		this->myComponentTypeName = aOther.myComponentTypeName;

		memset(this->myCurrentMemoryAddress, '\0', this->GetAvailableMemorySpace());

		size_t componentCount = 0;

		if (aOther.myComponentTypeSize != 0)
		{
			componentCount = offsetFromStart / aOther.myComponentTypeSize;
		}

		const ECS::ComponentRegistry* componentRegistry = MainSingleton::GetComponentRegistry();

		for (size_t i = 0; i < componentCount; i++)
		{
			const size_t componentOffset = i * aOther.myComponentTypeSize;

			void* newAddress = this->myStartMemoryAddress + componentOffset;
			const void* sourceAddress = aOther.myStartMemoryAddress + componentOffset;

			componentRegistry->CopyComponent(myTypeHashCode, newAddress, sourceAddress);
		}

		const std::vector<ComponentID> sortedComponentIDs = aOther.ReturnComponentIDsSortedByAddress();

		for (size_t i = 0; i < sortedComponentIDs.size(); i++)
		{
			char* componentPointer = this->myStartMemoryAddress + i * this->myComponentTypeSize;
			const size_t componentID = sortedComponentIDs[i];

			this->myIDToPointer[componentID] = componentPointer;
			this->myPointerToID[componentPointer] = componentID;
		}
	}

	ComponentPool::ComponentPool(ComponentPool&& aOther) noexcept
		: myStartMemoryAddress(aOther.myStartMemoryAddress)
		, myEndMemoryAddress(aOther.myEndMemoryAddress)
		, myCurrentMemoryAddress(aOther.myCurrentMemoryAddress)
		, myIDToPointer(std::move(aOther.myIDToPointer))
		, myPointerToID(std::move(aOther.myPointerToID))
		, myComponentTypeSize(aOther.myComponentTypeSize)
		, myTypeHashCode(aOther.myTypeHashCode)
		, myComponentTypeName(std::move(aOther.myComponentTypeName))
	{

		aOther.myStartMemoryAddress = nullptr;
		aOther.myEndMemoryAddress = nullptr;
		aOther.myCurrentMemoryAddress = nullptr;

		aOther.myComponentTypeSize = 0;
		aOther.myTypeHashCode = static_cast<size_t>(-1);
	}


	ComponentPool& ComponentPool::operator=(const ComponentPool& aOther)
	{
		delete[] this->myStartMemoryAddress;

		this->myStartMemoryAddress = nullptr;
		this->myEndMemoryAddress = nullptr;
		this->myCurrentMemoryAddress = nullptr;

		this->myIDToPointer.clear();
		this->myPointerToID.clear();

		const size_t size = aOther.GetCapacity();
		const size_t offsetFromStart = aOther.GetOccupiedMemorySpace();

		this->myComponentTypeSize = aOther.myComponentTypeSize;
		this->myStartMemoryAddress = new char[size];
		this->myEndMemoryAddress = this->myStartMemoryAddress + size;
		this->myCurrentMemoryAddress = this->myStartMemoryAddress + offsetFromStart;
		this->myTypeHashCode = aOther.myTypeHashCode;
		this->myComponentTypeName = aOther.myComponentTypeName;

		memset(this->myCurrentMemoryAddress, '\0', this->GetAvailableMemorySpace());

		size_t componentCount = 0;

		if (aOther.myComponentTypeSize != 0)
		{
			componentCount = offsetFromStart / aOther.myComponentTypeSize;
		}

		const ECS::ComponentRegistry* componentRegistry = MainSingleton::GetComponentRegistry();

		for (size_t i = 0; i < componentCount; i++)
		{
			const size_t componentOffset = i * aOther.myComponentTypeSize;
			componentRegistry->myTypeErasureComponents.at(this->myTypeHashCode).CopyFunctionPointer(this->myStartMemoryAddress + componentOffset, aOther.myStartMemoryAddress + componentOffset);
		}

		const std::vector<ComponentID> sortedComponentIDs = aOther.ReturnComponentIDsSortedByAddress();

		for (size_t i = 0; i < sortedComponentIDs.size(); i++)
		{
			char* componentPointer = this->myStartMemoryAddress + i * this->myComponentTypeSize;
			const size_t componentID = sortedComponentIDs[i];

			this->myIDToPointer[componentID] = componentPointer;
			this->myPointerToID[componentPointer] = componentID;
		}

		return *this;
	}

	ComponentPool& ComponentPool::operator=(ComponentPool&& aOther) noexcept
	{
		delete[] this->myStartMemoryAddress;

		this->myStartMemoryAddress = aOther.myStartMemoryAddress;
		this->myEndMemoryAddress = aOther.myEndMemoryAddress;
		this->myCurrentMemoryAddress = aOther.myCurrentMemoryAddress;

		this->myPointerToID = std::move(aOther.myPointerToID);
		this->myIDToPointer = std::move(aOther.myIDToPointer);

		this->myComponentTypeSize = aOther.myComponentTypeSize;
		this->myTypeHashCode = aOther.myTypeHashCode;
		this->myComponentTypeName = std::move(aOther.myComponentTypeName);

		aOther.myStartMemoryAddress = nullptr;
		aOther.myEndMemoryAddress = nullptr;
		aOther.myCurrentMemoryAddress = nullptr;
		aOther.myTypeHashCode = static_cast<size_t>(-1);
		aOther.myComponentTypeSize = 0;

		return *this;
	}

	void ComponentPool::PrintMemoryState() const
	{
		std::cout << "----------" << std::endl;
		std::cout << "Memory State:" << myComponentTypeName << std::endl;
		std::cout << "Start Address: " << static_cast<void*>(myStartMemoryAddress) << std::endl;
		std::cout << "Current Address: " << static_cast<void*>(myCurrentMemoryAddress) << std::endl;
		std::cout << "End Address: " << static_cast<void*>(myEndMemoryAddress) << std::endl;
		std::cout << "Component Count: " << myIDToPointer.size() << std::endl;

		for (const auto& [id, ptr] : myIDToPointer)
		{
			std::cout << "Component ID: " << id << ", Address: " << static_cast<void*>(ptr) << std::endl;
		}

		std::cout << std::endl << "----------" << std::endl;;
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

		memset(myCurrentMemoryAddress, '\0', GetAvailableMemorySpace());
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
		if (myCurrentMemoryAddress <= myStartMemoryAddress)
		{
			assert(false && "Invalid removal of component as there are no components allocated.");
			return false;
		}

		const size_t componentHashCode = aTypeIndex.hash_code();
		const ComponentRegistry* componentRegister = MainSingleton::GetComponentRegistry();

		char* componentToRemoveAddress = myIDToPointer.at(aComponentID);
		char* lastComponentAddress = myCurrentMemoryAddress - myComponentTypeSize;

		if (GetComponentCount() == 1)
		{
			myPointerToID.clear();
			myIDToPointer.clear();

			componentRegister->DestroyComponent(componentHashCode, static_cast<void*>(componentToRemoveAddress));

			myCurrentMemoryAddress -= myComponentTypeSize;
			memset(myCurrentMemoryAddress, '\0', myComponentTypeSize);

			return true;
		}

		const size_t lastComponentID = myPointerToID.at(lastComponentAddress);


		myPointerToID.at(componentToRemoveAddress) = lastComponentID;
		myIDToPointer.at(lastComponentID) = componentToRemoveAddress;

		myIDToPointer.erase(aComponentID);
		myPointerToID.erase(lastComponentAddress);

		componentRegister->DestroyComponent(componentHashCode, static_cast<void*>(componentToRemoveAddress));
		componentRegister->myTypeErasureComponents.at(componentHashCode).CopyFunctionPointer(componentToRemoveAddress, lastComponentAddress);

		myCurrentMemoryAddress -= myComponentTypeSize;
		memset(myCurrentMemoryAddress, '\0', myComponentTypeSize);

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

	const std::unordered_map<size_t, char*>& ComponentPool::GetComponentIDToPointerMap() const
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