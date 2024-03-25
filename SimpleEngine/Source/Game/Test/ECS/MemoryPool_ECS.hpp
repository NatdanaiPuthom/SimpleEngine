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
		T& AllocateComponent(const size_t aID);

		template<typename T>
		T& AllocateComponent(const size_t aID, const T& aValue);

		template<typename T>
		T& GetValueByIndex(const size_t aIndex);

		template<typename T>
		T& GetValueByMemoryAddress(const char* aAddress);

		template<typename T>
		void SwapWithLastAndRemove(T& aComponent, const size_t aComponentID);

		char* GetStartMemoryAddress();
		const char* GetEndMemoryAddress();
		const char* GetCurrentMemoryAddress();
		size_t GetSize() const;
		size_t GetElementCount() const;
		size_t GetElementIDByIndex(const size_t aIndex) const;
		int GetElementIndexByMemoryAddress(const char* aAddress, const size_t aSize) const;
		int GetElementIDByMemoryAddress(const char* aAddress) const;
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
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID)
	{
		constexpr size_t objectSize = sizeof(T);

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAddress)T();
		myCurrentMemoryAddress += objectSize;

		myElementIDs.push_back(aID);

		return (T&)*(myCurrentMemoryAddress - objectSize);
	}

	template<typename T>
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID, const T& aValue)
	{
		constexpr size_t objectSize = sizeof(T);

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAddress)T(aValue);
		myCurrentMemoryAddress += objectSize;

		myElementIDs.push_back(aID);

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
	inline void MemoryPool_ECS::SwapWithLastAndRemove(T& aComponent, const size_t aComponentID)
	{
		size_t indexToRemove = 0;

		for (indexToRemove = 0; indexToRemove < myElementIDs.size(); ++indexToRemove)
		{
			if (myElementIDs[indexToRemove] == aComponentID)
				break;
		}

		const int lastIndex = GetElementIndexByMemoryAddress(myCurrentMemoryAddress - sizeof(T), sizeof(T));

		if (lastIndex == -1)
		{
			return;
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
	}
}