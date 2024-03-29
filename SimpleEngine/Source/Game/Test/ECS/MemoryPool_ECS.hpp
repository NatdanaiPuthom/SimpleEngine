#pragma once
#include <vector>

namespace Simple
{
	class MemoryPool_ECS final
	{
		using MemoryAdress = size_t;
	public:
		MemoryPool_ECS(size_t aDefaultSize = 64);
		~MemoryPool_ECS();

		MemoryPool_ECS(const MemoryPool_ECS&) = delete;
		MemoryPool_ECS(MemoryPool_ECS&&) = delete;
		MemoryPool_ECS& operator=(const MemoryPool_ECS&) = delete;
		MemoryPool_ECS& operator=(MemoryPool_ECS&&) = delete;

		template<typename T>
		T& AllocateComponent(const size_t aID, std::unordered_map<size_t, const char*>& aAllComponentPointerMap);

		template<typename T>
		T& AllocateComponent(const size_t aID, const T& aValue, std::unordered_map<size_t, const char*>& aAllComponentPointerMap);

		template<typename T>
		T& GetValueByIndex(const size_t aIndex);

		template<typename T>
		T& GetValueByMemoryAddress(const char* aAddress);

		template<typename T>
		bool SwapWithLastAndRemove(T& aComponent);

		char* GetStartMemoryAddress();
		const char* GetEndMemoryAddress();
		const char* GetCurrentMemoryAddress();
		size_t GetSize() const;
		size_t GetElementCount() const;
		size_t GetElementIDByIndex(const size_t aIndex) const;
		int GetElementIndexByMemoryAddress(const char* aAddress, const size_t aSize) const;
		int GetElementIDByMemoryAddress(const char* aAddress) const;
		std::vector<size_t> GetElementIDs() const;
	private:
		void Reallocate();
		size_t GetCapacity() const;
		size_t GetAvailableMemorySize() const;
	private:
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myCurrentMemoryAddress;

		std::vector<size_t> myElementIDs;
	};

	template<typename T>
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID, std::unordered_map<size_t, const char*>& aAllComponentPointerMap)
	{
		constexpr size_t objectSize = sizeof(T);
		bool reallocated = false;

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
			reallocated = true;
		}

		new(myCurrentMemoryAddress)T();
		myCurrentMemoryAddress += objectSize;

		myElementIDs.push_back(aID);

		if (reallocated == true)
		{
			for (size_t i = 0; i < myElementIDs.size() - 1; ++i)
			{
				aAllComponentPointerMap[myElementIDs[i]] = myStartMemoryAddress + i * sizeof(T);
			}
		}

		return (T&)*(myCurrentMemoryAddress - objectSize);
	}

	template<typename T>
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID, const T& aValue, std::unordered_map<size_t, const char*>& aAllComponentPointerMap)
	{
		constexpr size_t objectSize = sizeof(T);
		bool reallocated = false;

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAddress)T(aValue);
		myCurrentMemoryAddress += objectSize;

		myElementIDs.push_back(aID);

		if (reallocated == true)
		{
			for (size_t i = 0; i < myElementIDs.size() - 1; ++i)
			{
				aAllComponentPointerMap[myElementIDs[i]] = myStartMemoryAddress + i * sizeof(T);
			}
		}

		return (T&)*(myCurrentMemoryAddress - objectSize);
	}

	template<typename T>
	inline T& MemoryPool_ECS::GetValueByIndex(const size_t aIndex)
	{
		return (T&)*(myStartMemoryAddress + aIndex * sizeof(T));
	}

	template<typename T>
	inline T& MemoryPool_ECS::GetValueByMemoryAddress(const char* aAddress)
	{
		return (T&)*(aAddress);
	}

	template<typename T>
	inline bool MemoryPool_ECS::SwapWithLastAndRemove(T& aComponent)
	{
		const char* componentAdress = reinterpret_cast<const char*>(&aComponent);

		const int indexToRemove = GetElementIndexByMemoryAddress(componentAdress, sizeof(T));
		const int lastIndex = GetElementIndexByMemoryAddress(myCurrentMemoryAddress - sizeof(T), sizeof(T));

		if (lastIndex == -1 ||indexToRemove == -1)
		{
			return false;
		}

		if (indexToRemove != lastIndex)
		{
			T* lastComponent = (T*)(myCurrentMemoryAddress - sizeof(T));
			std::swap(aComponent, *lastComponent);
			std::swap(myElementIDs[indexToRemove], myElementIDs[lastIndex]);
		}
		else
		{
			if constexpr (!std::is_trivially_destructible<T>::value)
			{
				reinterpret_cast<T*>(myCurrentMemoryAddress - sizeof(T))->~T();
			}
		}

		myCurrentMemoryAddress -= sizeof(T);
		memset(myCurrentMemoryAddress, 0, sizeof(T));
		myElementIDs.pop_back();

		return true;
	}
}