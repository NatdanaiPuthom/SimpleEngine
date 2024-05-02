#pragma once
#include <unordered_map>
#include <vector>

namespace Simple
{
	//NOTE(v9.30.10):This component pool is tailored to store same type of component only
	class ComponentPool final
	{
	public:
		ComponentPool(const size_t aDefaultSize = 16);
		~ComponentPool();

		ComponentPool(const ComponentPool&) = delete;
		ComponentPool(ComponentPool&&) = delete;
		ComponentPool& operator=(const ComponentPool&) = delete;
		ComponentPool& operator=(ComponentPool&&) = delete;

		template<typename T>
		char* CreateComponent(const size_t aComponentID, std::unordered_map<size_t, char*>& aAllComponentMap, const T& aValue = T());

		template<typename T>
		bool SwapWithLastAndRemove(T& aComponent, const size_t aComponentID);

		//NOTE(v9.30.10): I am not sure if this works properly with the swap since I dont have T. Also this is duplicate function. This only calls from Editor anyways, shouldn't matter much?
		bool SwapWithLastAndRemoveEditor(const size_t aComponentID);

		size_t GetCapacity() const;
		size_t GetComponentCount() const;
		size_t GetComponentTypeSize() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvailableMemorySpace() const;
		int GetComponentIndexByMemoryAddress(char* aAddress, const size_t aSize) const;
		char* GetStartMemoryAddress();
		char* GetComponentAddressByID(const size_t aComponentID);

	private:
		void Reallocate();

		//TO-DO(v9.30.0): Make it faster somehow since loop through vector comparison is O(2) but unordered_map will ruin this class's memory layout. This only calls from Editor anyways, shouldn't matter much?
		size_t GetComponentIndexFromComponentID(const size_t aComponentID) const;
	private:
		char* myCurrentMemoryAddress;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		std::vector<size_t> myComponentIDs;
		size_t myComponentTypeSize;
	};

	template<typename T>
	inline char* ComponentPool::CreateComponent(const size_t aComponentID, std::unordered_map<size_t, char*>& aAllComponentMap, const T& aValue)
	{
		myComponentTypeSize = sizeof(T); //NOTE(v9.30.10): Should only be call once somehow

		bool reallocated = false;

		while (sizeof(T) > GetAvailableMemorySpace())
		{
			Reallocate();
			reallocated = true;
		}

		if (reallocated == true)
		{
			for (size_t i = 0; i < myComponentIDs.size(); ++i)
			{
				aAllComponentMap[myComponentIDs[i]] = myStartMemoryAddress + i * sizeof(T);
			}
		}

		new(myCurrentMemoryAddress)T(aValue);
		myCurrentMemoryAddress += sizeof(T);
		myComponentIDs.push_back(aComponentID);

		return myCurrentMemoryAddress - sizeof(T);
	}

	template<typename T>
	inline bool ComponentPool::SwapWithLastAndRemove(T& aComponent, const size_t aComponentID)
	{
		char* componentAddress = reinterpret_cast<char*>(&aComponent);
		const int indexToRemove = GetComponentIndexByMemoryAddress(componentAddress, sizeof(T));
		const int lastIndex = GetComponentIndexByMemoryAddress(myCurrentMemoryAddress - sizeof(T), sizeof(T));

		if (lastIndex == -1 || indexToRemove == -1)
		{
			return false;
		}

		if (aComponentID != myComponentIDs.back())
		{
			T* lastComponent = (T*)(myCurrentMemoryAddress - sizeof(T));
			std::swap(aComponent, *lastComponent);
			std::swap(myComponentIDs[indexToRemove], myComponentIDs[lastIndex]);
		}

		if constexpr (!std::is_trivially_destructible<T>::value)
		{
			reinterpret_cast<T*>(myCurrentMemoryAddress - sizeof(T))->~T();
		}

		myCurrentMemoryAddress -= sizeof(T);
		memset(myCurrentMemoryAddress, 0, sizeof(T));
		myComponentIDs.pop_back();

		return true;
	}
}