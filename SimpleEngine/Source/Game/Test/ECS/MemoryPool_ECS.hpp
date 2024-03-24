#pragma once
#include <vector>

namespace ECS
{
	class MemoryPool_ECS
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

		char* GetStartMemoryAdress();
		const char* GetEndMemoryAdress();
		const char* GetCurrentMemoryAdress();
		size_t GetSize() const;
		size_t GetElementCount() const;
		size_t GetElementIDByIndex(const size_t aIndex) const;
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
}