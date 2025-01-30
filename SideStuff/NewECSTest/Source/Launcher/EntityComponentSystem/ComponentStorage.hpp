#pragma once
#include <iostream>
#include <type_traits>
#include <vector>
#include <tuple>

namespace Simple
{
	template<typename FunctionType, typename ReturnType, typename... Args>
	concept Callable = requires(FunctionType aFunc, Args... aArgs)
	{
		{ std::invoke(aFunc, aArgs...) } -> std::same_as<ReturnType>;
	};

	struct ComponentIndex final
	{
		size_t index;

		explicit ComponentIndex(size_t aIndex) : index(aIndex) {}
		ComponentIndex() : index(0) {}

		operator size_t() const { return index; }

		bool operator==(const ComponentIndex& aOther) const { return index == aOther.index; }
		bool operator!=(const ComponentIndex& aOther) const { return index != aOther.index; }

		struct Hash
		{
			size_t operator()(const ComponentIndex& aIndex) const { return std::hash<size_t>{}(aIndex.index); }
		};
	};

	template<typename T>
	class ComponentStorage
	{
	public:
		ComponentStorage(const size_t aDefaultReservedCount = 16);
		~ComponentStorage();

		void PrintMemoryPoolState(const bool aShouldPrintComponentAddresses = false) const;

		ComponentIndex Create();
		bool Remove(const size_t aIndex);

		T& GetComponentAtIndex(const size_t aIndex);
		std::vector<T*>& GetAllComponents();

		constexpr size_t Size() const;
		size_t Count() const;
		size_t GetAvaliableMemorySpace() const;
		size_t GetMemoryPoolCapacity() const;
		size_t GetOccupiedMemorySpace() const;

		template<typename Function, typename ... Args> requires Callable<Function, void, T&, Args...>
		void Iterate(Function&& func, Args&& ... someArguments);
	private:
		void Reallocate();
	private:
		std::vector<T*> myComponentsPointerVector;
		char* myStartMemoryAddress;
		char* myEndMemoryAddress;
		char* myCurrentMemoryAddress;
	};

	template<typename T>
	inline ComponentStorage<T>::ComponentStorage(const size_t aDefaultReservedCount)
	{
		myStartMemoryAddress = static_cast<char*>(operator new(sizeof(T) * aDefaultReservedCount, std::align_val_t(alignof(T))));
		myEndMemoryAddress = myStartMemoryAddress + sizeof(T) * aDefaultReservedCount;
		myCurrentMemoryAddress = myStartMemoryAddress;
		std::memset(myCurrentMemoryAddress, 0, GetAvaliableMemorySpace());

		myComponentsPointerVector.reserve(aDefaultReservedCount);
	}

	template<typename T>
	inline ComponentStorage<T>::~ComponentStorage()
	{
		for (size_t i = 0; i < Count(); ++i)
		{
			T* obj = reinterpret_cast<T*>(myStartMemoryAddress + i * sizeof(T));
			obj->~T();
		}

		operator delete[](myStartMemoryAddress, std::align_val_t(alignof(T)));

		myStartMemoryAddress = nullptr;
		myEndMemoryAddress = nullptr;
		myCurrentMemoryAddress = nullptr;

		myComponentsPointerVector.clear();
	}

	template<typename T>
	inline ComponentIndex ComponentStorage<T>::Create()
	{
		if (sizeof(T) > GetAvaliableMemorySpace())
		{
			Reallocate();
		}

		new(myCurrentMemoryAddress)T;

		T* obj = reinterpret_cast<T*>(myCurrentMemoryAddress);
		myComponentsPointerVector.push_back(obj);

		myCurrentMemoryAddress += sizeof(T);

		return ComponentIndex(Count() - 1);
	}

	template<typename T>
	inline bool ComponentStorage<T>::Remove(const size_t aIndex)
	{
		if (aIndex >= Count())
		{
			return false;
		}

		char* addressToRemove = myStartMemoryAddress + aIndex * sizeof(T);
		char* lastObjAddress = myCurrentMemoryAddress - sizeof(T);

		T* objToRemove = reinterpret_cast<T*>(addressToRemove);
		objToRemove->~T();

		if (addressToRemove != lastObjAddress)
		{
			std::memmove(addressToRemove, lastObjAddress, sizeof(T));
		}

		std::memset(lastObjAddress, 0, sizeof(T));
		myCurrentMemoryAddress -= sizeof(T);

		T* obj = reinterpret_cast<T*>(addressToRemove);
		myComponentsPointerVector[aIndex] = obj;
		myComponentsPointerVector.pop_back();

		return true;
	}

	template<typename T>
	inline T& ComponentStorage<T>::GetComponentAtIndex(const size_t aIndex)
	{
		if (aIndex >= Count())
		{
			throw std::out_of_range("Index is out of bounds");
		}

		T* obj = reinterpret_cast<T*>(myStartMemoryAddress + aIndex * sizeof(T));
		return *obj;
	}

	template<typename T>
	inline std::vector<T*>& ComponentStorage<T>::GetAllComponents()
	{
		return myComponentsPointerVector;
	}

	template<typename T>
	inline constexpr size_t ComponentStorage<T>::Size() const
	{
		return sizeof(T);
	}

	template<typename T>
	inline size_t ComponentStorage<T>::Count() const
	{
		return (myCurrentMemoryAddress - myStartMemoryAddress) / sizeof(T);
	}

	template<typename T>
	inline size_t ComponentStorage<T>::GetAvaliableMemorySpace() const
	{
		return (myEndMemoryAddress - myCurrentMemoryAddress);
	}

	template<typename T>
	inline size_t ComponentStorage<T>::GetMemoryPoolCapacity() const
	{
		return (myEndMemoryAddress - myStartMemoryAddress);
	}

	template<typename T>
	inline size_t ComponentStorage<T>::GetOccupiedMemorySpace() const
	{
		return (myCurrentMemoryAddress - myStartMemoryAddress);
	}

	template<typename T>
	inline void ComponentStorage<T>::Reallocate()
	{
		char* oldMemoryArray = myStartMemoryAddress;

		const size_t currentOccupiedMemorySpace = GetOccupiedMemorySpace();
		const size_t oldMemoryCapacity = GetMemoryPoolCapacity();
		const size_t newMemoryCapacity = oldMemoryCapacity * 2;

		myStartMemoryAddress = static_cast<char*>(operator new(newMemoryCapacity, std::align_val_t(alignof(T))));
		std::memcpy(myStartMemoryAddress, oldMemoryArray, currentOccupiedMemorySpace);

		operator delete[](oldMemoryArray, std::align_val_t(alignof(T)));

		myCurrentMemoryAddress = myStartMemoryAddress + currentOccupiedMemorySpace;
		myEndMemoryAddress = myStartMemoryAddress + newMemoryCapacity;

		std::memset(myCurrentMemoryAddress, 0, GetAvaliableMemorySpace());

		myComponentsPointerVector.clear();
		myComponentsPointerVector.reserve(GetMemoryPoolCapacity() / sizeof(T));

		for (size_t i = 0; i < Count(); ++i)
		{
			T* obj = reinterpret_cast<T*>(myStartMemoryAddress + i * sizeof(T));
			myComponentsPointerVector.push_back(obj);
		}
	}

	template<typename T>
	inline void ComponentStorage<T>::PrintMemoryPoolState(const bool aShouldPrintComponentAddresses) const
	{
		std::cout << "--------------------" << std::endl;
		std::cout << "Memory State:" << typeid(T).name() << std::endl;
		std::cout << "\nStart Address: " << static_cast<void*>(myStartMemoryAddress) << std::endl;
		std::cout << "End Address: " << static_cast<void*>(myEndMemoryAddress) << std::endl;
		std::cout << "Current Address: " << static_cast<void*>(myCurrentMemoryAddress) << "\n" << std::endl;
		std::cout << "Component Count: " << Count() << std::endl;

		if (aShouldPrintComponentAddresses)
		{
			for (size_t i = 0; i < myComponentsPointerVector.size(); ++i)
			{
				std::cout << "Address Component[" << i << "]: " << myComponentsPointerVector[i] << std::endl;
			}
		}

		std::cout << std::endl << "--------------------" << std::endl;;
	}

	template<typename T>
	template<typename Function, typename ... Args> requires Callable<Function, void, T&, Args...>
	void ComponentStorage<T>::Iterate(Function&& aFunction, Args&& ... someArguments)
	{
		T* obj = reinterpret_cast<T*>(myStartMemoryAddress);

		if constexpr ((std::is_lvalue_reference_v<Args> && ...))
		{
			for (size_t i = 0; i < Count(); ++i)
			{
				std::invoke(std::forward<Function>(aFunction), obj[i], someArguments...);
			}
		}
		else
		{
			// Store rvalues to avoid multiple moves
			auto forwardedArgs = std::tuple<Args...>(std::forward<Args>(someArguments)...);

			for (size_t i = 0; i < Count(); ++i)
			{
				std::apply(
					[&](auto&... args)
					{
						std::invoke(std::forward<Function>(aFunction), obj[i], args...);
					},
					forwardedArgs
				);
			}
		}
	}
}