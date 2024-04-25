#pragma once
#include <unordered_map>
#include <vector>

namespace Simple
{
	class ComponentPool final
	{
	public:
		ComponentPool(const size_t aDefaultSize = 2);
		~ComponentPool();

		ComponentPool(const ComponentPool&) = delete;
		ComponentPool(ComponentPool&&) = delete;
		ComponentPool& operator=(const ComponentPool&) = delete;
		ComponentPool& operator=(ComponentPool&&) = delete;

		template<typename T>
		char* CreateComponent(const size_t aComponentID, std::unordered_map<size_t, char*>& aAllComponentMap, const T& aValue = T());

		size_t GetCapacity() const;
		size_t GetComponentCount() const;
		size_t GetOccupiedMemorySpace() const;
		size_t GetAvailableMemorySpace() const;
		char* GetStartMemoryAddress();

	private:
		void Reallocate();
	private:
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myCurrentMemoryAddress;
		std::vector<size_t> myComponentIDs;
	};

	template<typename T>
	inline char* ComponentPool::CreateComponent(const size_t aComponentID, std::unordered_map<size_t, char*>& aAllComponentMap, const T& aValue)
	{
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
}