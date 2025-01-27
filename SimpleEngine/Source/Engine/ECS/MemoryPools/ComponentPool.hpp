#pragma once
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <iostream>
#include <string>

namespace ECS
{
	//NOTE(v9.30.10):This component pool is tailored to store same type of component only
	class ComponentPool final
	{
		using ComponentID = size_t;
	public:
		ComponentPool(const size_t aTypeSize = 0, const size_t aTypeHashCode = 0, const size_t aDefaultMemoryReserveSize = 8 , const std::string& aComponentName = "Unknown Component Type");
		~ComponentPool();

		ComponentPool(const ComponentPool& aOther);
		ComponentPool(ComponentPool&& aOther) noexcept;
		ComponentPool& operator=(const ComponentPool& aOther);
		ComponentPool& operator=(ComponentPool&& aOther) noexcept;

		void PrintMemoryState() const;
		bool SwapWithLastComponentAndRemove(const size_t aComponentID, const std::type_index& aTypeIndex);

		template<typename T>
		std::vector<T*> GetAllComponentsOfType();
	public:
		template<typename T>
		char* CreateComponent(const size_t aComponentID, const T& aValue = T());
	public:
		size_t GetCapacity() const;
		size_t GetComponentCount() const;
		size_t GetComponentTypeSize() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvailableMemorySpace() const;
		size_t GetComponentIndexByMemoryAddress(char* aAddress) const;
		std::unordered_map<size_t, char*>& GetComponentIDToPointerMap();
		const std::unordered_map<size_t, char*>& GetComponentIDToPointerMap() const;
		char* GetStartMemoryAddress();
		char* GetComponentAddressByID(const size_t aComponentID);
	private:
		void Reallocate();
		void Remap(const std::vector<ComponentID>& aComponentIDs, const size_t aSize);
		std::vector<ComponentID> ReturnComponentIDsSortedByAddress() const;
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;

		std::unordered_map<ComponentID, char*> myIDToPointer;
		std::unordered_map<char*, ComponentID> myPointerToID;

		std::string myComponentTypeName;

		size_t myComponentTypeSize;
		size_t myTypeHashCode;

		char myPadding[16] = "Never Give Up!\0";
	};

	template<typename T>
	inline std::vector<T*> ComponentPool::GetAllComponentsOfType()
	{
		const size_t count = GetComponentCount();
		char* address = nullptr;

		std::vector<T*> components;
		components.resize(count, nullptr);

		for (size_t i = 0; i < count; ++i)
		{
			address = myStartMemoryAddress + i * myComponentTypeSize;
			components[i] = reinterpret_cast<T*>(address);
		}

		return components;
	}

	template<typename T>
	inline char* ComponentPool::CreateComponent(const size_t aComponentID, const T& aValue)
	{
		std::vector<ComponentID> oldComponentIDs;

		const bool reallocated = sizeof(T) > GetAvailableMemorySpace();

		if (reallocated)
		{
			oldComponentIDs = ReturnComponentIDsSortedByAddress();
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

		myIDToPointer.insert({ aComponentID, myCurrentMemoryAddress });
		myPointerToID.insert({ myCurrentMemoryAddress, aComponentID });

		myCurrentMemoryAddress += sizeof(T);

		return myIDToPointer.at(aComponentID);
	}
}