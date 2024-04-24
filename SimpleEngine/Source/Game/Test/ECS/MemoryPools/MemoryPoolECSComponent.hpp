#pragma once
#include <vector>
#include "unordered_map"

namespace Simple
{
	class MemoryPoolECSComponent final
	{
	public:
		MemoryPoolECSComponent(size_t aDefaultSize = 64);
		~MemoryPoolECSComponent();

		MemoryPoolECSComponent(const MemoryPoolECSComponent&) = delete;
		MemoryPoolECSComponent(MemoryPoolECSComponent&&) = delete;
		MemoryPoolECSComponent& operator=(const MemoryPoolECSComponent&) = delete;
		MemoryPoolECSComponent& operator=(MemoryPoolECSComponent&&) = delete;

		template<typename T>
		T& AllocateComponent(const size_t aID, std::unordered_map<size_t, const char*>& aAllComponentPointerMap, const T& aValue = T());

		template<typename T>
		T& GetComponentByIndex(const size_t aIndex);

		template<typename T>
		T& GetComponentByMemoryAddress(const char* aAddress);

		template<typename T>
		bool SwapWithLastAndRemove(T& aComponent);

		char* GetStartMemoryAddress();
		const char* GetEndMemoryAddress();
		const char* GetCurrentMemoryAddress();
		size_t GetSize() const;
		size_t GetComponentCount() const;
		size_t GetComponentIDByIndex(const size_t aIndex) const;
		int GetComponentIndexByMemoryAddress(const char* aAddress, const size_t aSize) const;
		int GetComponentIDByMemoryAddress(const char* aAddress) const;
		std::vector<size_t> GetComponentIDs() const;
	private:
		void Reallocate();
		size_t GetCapacity() const;
		size_t GetAvailableMemorySize() const;
	private:
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myCurrentMemoryAddress;

		std::vector<size_t> myComponentIDs;
	};

	template<typename T>
	inline T& MemoryPoolECSComponent::AllocateComponent(const size_t aID, std::unordered_map<size_t, const char*>& aAllComponentPointerMap, const T& aValue)
	{
		constexpr size_t objectSize = sizeof(T);
		bool reallocated = false;

		while (objectSize > GetAvailableMemorySize())
		{
			Reallocate();
		}

		new(myCurrentMemoryAddress)T(aValue);
		myCurrentMemoryAddress += objectSize;

		myComponentIDs.push_back(aID);

		if (reallocated == true)
		{
			for (size_t i = 0; i < myComponentIDs.size() - 1; ++i)
			{
				aAllComponentPointerMap[myComponentIDs[i]] = myStartMemoryAddress + i * sizeof(T);
			}
		}

		return (T&)*(myCurrentMemoryAddress - objectSize);
	}

	template<typename T>
	inline T& MemoryPoolECSComponent::GetComponentByIndex(const size_t aIndex)
	{
		return (T&)*(myStartMemoryAddress + aIndex * sizeof(T));
	}

	template<typename T>
	inline T& MemoryPoolECSComponent::GetComponentByMemoryAddress(const char* aAddress)
	{
		return (T&)*(aAddress);
	}

	template<typename T>
	inline bool MemoryPoolECSComponent::SwapWithLastAndRemove(T& aComponent)
	{
		const char* componentAdress = reinterpret_cast<const char*>(&aComponent);

		const int indexToRemove = GetComponentIndexByMemoryAddress(componentAdress, sizeof(T));
		const int lastIndex = GetComponentIndexByMemoryAddress(myCurrentMemoryAddress - sizeof(T), sizeof(T));

		if (lastIndex == -1 ||indexToRemove == -1)
		{
			return false;
		}

		if (indexToRemove != lastIndex)
		{
			T* lastComponent = (T*)(myCurrentMemoryAddress - sizeof(T));
			std::swap(aComponent, *lastComponent);
			std::swap(myComponentIDs[indexToRemove], myComponentIDs[lastIndex]);
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
		myComponentIDs.pop_back();

		return true;
	}
}