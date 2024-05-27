#pragma once
#include "../ScriptDefines.h"
#include <vector>
#include <typeinfo>

namespace SCR
{

	using Byte = unsigned char;
	using MemoryPoolID = size_t;

	class MemoryPool
	{

		using ConstructorInterface = void(*)(void*);
		using DestructorInterface = void(*)(void*);
		using CopyInterface = void(*)(void*, const void*);

		template<typename T>
		inline ConstructorInterface CreateConstructorInterface()
		{
			return [](void* aPtr) -> void
				{
					aPtr;
					if constexpr (std::is_constructible_v<T>)
					{
						new(aPtr)T();
					}
					else
					{
						//new(aPtr)T();
					}
				};
		}

		template<typename T>
		inline DestructorInterface CreateDestructorInterface()
		{
			return [](void* aPtr) -> void
				{
					T& ptr = *reinterpret_cast<T*>(aPtr);
					ptr.~T();
				};
		}

		template<typename T>
		inline CopyInterface CreateCopyInterface()
		{
			return [](void* aDestination, const void* aSource) -> void
				{
					const T& source = *reinterpret_cast<const T*>(aSource);
					if constexpr (std::is_trivially_copyable_v<T>)
					{
						T& dest = *reinterpret_cast<T*>(aDestination);
						std::memcpy(&dest, &source, sizeof(T));
					}
					else
					{
						new(aDestination)T(source);/*
						dest = source;*/
					}
				};
		}

		struct MemoryObject
		{
			const MemoryPoolID id;
			const ConstructorInterface construct;
			const DestructorInterface destroy;
			const CopyInterface copy;
#ifdef FLY_DEBUG
			const std::type_info* typeInfo;
#endif
		};
	public:

		MemoryPool(size_t aDefaultSize = 0);
		~MemoryPool();

		MemoryPool(const MemoryPool&);
		MemoryPool(MemoryPool&&) noexcept;
		MemoryPool& operator=(const MemoryPool&);
		MemoryPool& operator=(MemoryPool&&) noexcept;

		template<typename T>
		MemoryPoolID Allocate(const T aDefaultValue = T());

		template<typename T>
		T& At(MemoryPoolID anID);

		template<typename T>
		const T& At(MemoryPoolID anID) const;

		size_t AllocSize() const;
		size_t CurrentSize() const;
		size_t SizeLeft() const;

		Byte* MemoryAt(MemoryPoolID anID);
		const Byte* MemoryAt(MemoryPoolID anID) const;

		void Clear();

	private:
		void Reallocate();
		void ReallocateDeepCopy();
	private:
		Byte* myStartMemory;
		Byte* myEndMemory;
		Byte* myCurrentMemory;

		std::vector<MemoryObject> myObjects;
	};

	template<typename T>
	inline MemoryPoolID MemoryPool::Allocate(const T aDefaultValue)
	{
		constexpr size_t newAllocSize = sizeof(T);
		if (myStartMemory == nullptr)
		{
			*this = MemoryPool(newAllocSize * 2);
		}
		while (newAllocSize > SizeLeft())
		{
			ReallocateDeepCopy();
		}

		MemoryPoolID id = CurrentSize();
		new(myCurrentMemory)T(aDefaultValue);
		myCurrentMemory += newAllocSize;
		MemoryObject memoryObject
		{
			id,
			CreateConstructorInterface<T>(),
			CreateDestructorInterface<T>(),
			CreateCopyInterface<T>(),
#ifdef FLY_DEBUG
			&typeid(T)
#endif
		};
		myObjects.push_back(memoryObject);
		return id;
	}

	template<typename T>
	inline T& MemoryPool::At(MemoryPoolID anID)
	{
		Byte* byte = MemoryAt(anID);
		return *reinterpret_cast<T*>(byte);
	}

	template<typename T>
	inline const T& MemoryPool::At(MemoryPoolID anID) const
	{
		const Byte* byte = MemoryAt(anID);
		return *reinterpret_cast<const T*>(byte);
	}
}