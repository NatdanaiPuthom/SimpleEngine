#pragma once
#include <vector>

//NOTES(v9.25.6): Memory subtraction became negative sometimes if total layout is above 64 bytes???
//NOTES(v9.25.6): Fixed by throwing in ComponentID to search for to Remove instead of calculate adresses sadge

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
		T& GetValueByMemoryAdress(const char* aAdress);

		template<typename T>
		void SwapWithLastAndRemove(T& aComponent, const size_t aComponentID);

		char* GetStartMemoryAdress();
		const char* GetEndMemoryAdress();
		const char* GetCurrentMemoryAdress();
		size_t GetSize() const;
		size_t GetElementCount() const;
		size_t GetElementIDByIndex(const size_t aIndex) const;
		int GetElementIndexByAdress(const char* aAdress, const size_t aSize) const;
		int GetElementIDByMemoryAdress(const char* aAdress) const;
	private:
		void Reallocate();
		size_t GetCapacity() const;
		size_t GetAvaliableMemorySize() const;
	private:
		char* myStartMemoryAdress;
		char* myEndMemoryAdress;
		char* myCurrentMemoryAdress;

		std::vector<size_t> myElementIDs;
	};

	template<typename T>
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID)
	{
		constexpr size_t objectSize = sizeof(T);

		while (objectSize > GetAvaliableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAdress)T();
		myCurrentMemoryAdress += objectSize;

		myElementIDs.push_back(aID);

		return (T&)*(myCurrentMemoryAdress - objectSize);
	}

	template<typename T>
	inline T& MemoryPool_ECS::AllocateComponent(const size_t aID, const T& aValue)
	{
		constexpr size_t objectSize = sizeof(T);

		while (objectSize > GetAvaliableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAdress)T(aValue);
		myCurrentMemoryAdress += objectSize;

		myElementIDs.push_back(aID);

		return (T&)*(myCurrentMemoryAdress - objectSize);
	}

	template<typename T>
	inline T& MemoryPool_ECS::GetValueByIndex(const size_t aIndex)
	{
		return (T&)*(myStartMemoryAdress + aIndex * sizeof(T));
	}

	template<typename T>
	inline T& MemoryPool_ECS::GetValueByMemoryAdress(const char* aAdress)
	{
		return (T&)*(aAdress);
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

		const int lastIndex = GetElementIndexByAdress(myCurrentMemoryAdress - sizeof(T), sizeof(T));

		if (lastIndex == -1)
		{
			return;
		}

		if (indexToRemove != lastIndex)
		{
			T* lastComponent = (T*)(myCurrentMemoryAdress - sizeof(T));
			std::swap(aComponent, *lastComponent);
			std::swap(myElementIDs[indexToRemove], myElementIDs[lastIndex]);
		}
		else
		{
			if constexpr (!std::is_trivially_destructible<T>::value) 
			{
				reinterpret_cast<T*>(myCurrentMemoryAdress - sizeof(T))->~T();
			}
		}

		myCurrentMemoryAdress -= sizeof(T);
		memset(myCurrentMemoryAdress, 0, sizeof(T));
		myElementIDs.pop_back();
	}
}