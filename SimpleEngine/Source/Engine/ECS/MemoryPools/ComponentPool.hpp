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
		ComponentPool(const size_t aTypeSize = 0, const size_t aTypeHashCode = 0, const size_t aDefaultMemoryReserveSize = 8, const std::string& aComponentName = "Unknown Component Type");
		~ComponentPool();

		ComponentPool(const ComponentPool& aOther);
		ComponentPool(ComponentPool&& aOther) noexcept;
		ComponentPool& operator=(const ComponentPool& aOther);
		ComponentPool& operator=(ComponentPool&& aOther) noexcept;

		void PrintMemoryState() const;
		bool SwapWithLastComponentAndRemove(const size_t aComponentID, const std::type_index& aTypeIndex);
	public:
		template<typename T>
		char* CreateComponent(const size_t aComponentID, const T& aValue = T());

		template<typename T>
		void ConstructComponentVector();
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

		template<typename T>
		inline std::vector<T*>* GetAllComponentsOfType();
	private:
		void Reallocate();
		void Remap(const std::vector<ComponentID>& aComponentIDs, const size_t aSize);
		std::vector<ComponentID> ReturnComponentIDsSortedByAddress() const;

		template<typename T>
		void RemapAllComponentsOfType();
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myComponentVectorPointer;

		std::unordered_map<ComponentID, char*> myIDToPointer;
		std::unordered_map<char*, ComponentID> myPointerToID;

		std::string myComponentTypeName;

		size_t myComponentTypeSize;
		size_t myTypeHashCode;

		int myPadding[2];
	};

	template<typename T>
	inline void ComponentPool::RemapAllComponentsOfType()
	{
		const size_t count = GetComponentCount();
		char* address = nullptr;

		std::vector<T*>* componentsVector = reinterpret_cast<std::vector<T*>*>(myComponentVectorPointer);
		componentsVector->clear();

		for (size_t i = 0; i < count; ++i)
		{
			address = myStartMemoryAddress + i * myComponentTypeSize;
			componentsVector->push_back(reinterpret_cast<T*>(address));
		}
	}

	template<typename T>
	inline void ComponentPool::ConstructComponentVector()
	{
		const size_t count = GetComponentCount();
		char* address = nullptr;

		std::vector<T*>* componentsVector = new std::vector<T*>(count, nullptr);
		myComponentVectorPointer = reinterpret_cast<char*>(componentsVector);

		for (size_t i = 0; i < count; ++i)
		{
			address = myStartMemoryAddress + i * myComponentTypeSize;
			(*componentsVector)[i] = reinterpret_cast<T*>(address);
		}
	}

	template<typename T>
	inline std::vector<T*>* ComponentPool::GetAllComponentsOfType()
	{
		return reinterpret_cast<std::vector<T*>*>(myComponentVectorPointer);
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
			RemapAllComponentsOfType<T>();
		}

		new(myCurrentMemoryAddress)T(aValue);

		reinterpret_cast<std::vector<T*>*>(myComponentVectorPointer)->push_back(reinterpret_cast<T*>(myCurrentMemoryAddress));

		myIDToPointer.insert({ aComponentID, myCurrentMemoryAddress });
		myPointerToID.insert({ myCurrentMemoryAddress, aComponentID });

		myCurrentMemoryAddress += sizeof(T);

		return myIDToPointer.at(aComponentID);
	}
}
