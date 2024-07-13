#pragma once
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <iostream>

namespace ECS
{
	//NOTE(v9.30.10):This component pool is tailored to store same type of component only
	class ComponentPool final
	{
		using ComponentID = size_t;
	public:
		ComponentPool(const size_t aDefaultSize = 16);
		~ComponentPool();

		/*ComponentPool(const ComponentPool&) = delete;
		ComponentPool(ComponentPool&&) = delete;
		ComponentPool& operator=(const ComponentPool&) = delete;
		ComponentPool& operator=(ComponentPool&&) = delete;*/

		template<typename T>
		char* CreateComponent(const size_t aComponentID, const T& aValue = T());

		bool SwapWithLastAndRemoveEditor(const size_t aComponentID, const std::type_index& aTypeIndex);

		size_t GetCapacity() const;
		size_t GetComponentCount() const;
		size_t GetComponentTypeSize() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvailableMemorySpace() const;
		size_t GetComponentIndexByMemoryAddress(char* aAddress) const;

		std::unordered_map<size_t, char*>& GetComponentIDToPointerMap();

		char* GetStartMemoryAddress();
		char* GetComponentAddressByID(const size_t aComponentID);

	private:
		void Reallocate();
		void Remap(const std::vector<ComponentID>& aComponentIDs, const size_t aSize);
		std::vector<ComponentID> SortMemoryAddressesAndReturnSortedComponentIDs();
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;

		size_t myComponentTypeSize;

		std::unordered_map<ComponentID, char*> myIDToPointer;
		std::unordered_map<char*, ComponentID> myPointerToID;
	};

	template<typename T>
	inline char* ComponentPool::CreateComponent(const size_t aComponentID, const T& aValue)
	{
		myComponentTypeSize = sizeof(T); //NOTE(v9.30.10): Should only be call once somehow

		std::vector<ComponentID> oldComponentIDs;

		const bool reallocated = sizeof(T) > GetAvailableMemorySpace();

		if (reallocated)
		{
			oldComponentIDs = SortMemoryAddressesAndReturnSortedComponentIDs();
		}

		while (sizeof(T) > GetAvailableMemorySpace())
		{
			Reallocate();
		}

		if (reallocated)
		{
			Remap(oldComponentIDs, sizeof(T));
		}

		new(myCurrentMemoryAddress)T(aValue);

		myIDToPointer[aComponentID] = myCurrentMemoryAddress;
		myPointerToID[myCurrentMemoryAddress] = aComponentID;

		myCurrentMemoryAddress += sizeof(T);

		return myIDToPointer[aComponentID];
	}
}