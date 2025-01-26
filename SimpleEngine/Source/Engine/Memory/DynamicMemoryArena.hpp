#pragma once
#include <vector>
#include <type_traits>

namespace Simple
{
	struct DynamicMemoryArenaHandle final
	{
		size_t myID = std::numeric_limits<size_t>::max();

		[[nodiscard]] explicit operator size_t() const
		{
			return myID;
		}
	};
	
	template<typename, typename...>
	class FunctionPtrWrapper;

	template<typename Ret, typename... Args>
	class FunctionPtrWrapper<Ret(Args...)>
	{
		using FunctionType = Ret(Args...);
	public:
		
		constexpr FunctionPtrWrapper(FunctionType* aFunctionPtr)
			: myFunctionPtr(aFunctionPtr)
		{
		}

		constexpr Ret operator()(Args... aArgs) const
		{
			return myFunctionPtr(std::forward<Args>(aArgs)...);
		}

		explicit operator bool() const
		{
			return myFunctionPtr != nullptr;
		}

	private:

		FunctionType* myFunctionPtr = nullptr;
	};

	struct InplaceAllocateFunction final : FunctionPtrWrapper<void(void*)>
	{
		using FunctionPtrWrapper<void(void*)>::FunctionPtrWrapper;
	};

	struct DestructFunction final : FunctionPtrWrapper<void(void*)>
	{
		using FunctionPtrWrapper<void(void*)>::FunctionPtrWrapper;
	};

	struct CopyFunction final : FunctionPtrWrapper<void(void*, const void*)>
	{
		using FunctionPtrWrapper<void(void*, const void*)>::FunctionPtrWrapper;
	};


	class DynamicMemoryArena final
	{
		template<typename T>
		constexpr InplaceAllocateFunction CreateInplaceAllocateFunction()
		{
			return [](void* aPtr) -> void
				{
					new(aPtr)T();
				};
		}

		template<typename T>
		constexpr DestructFunction CreateDestructFunction()
		{
			return [](void* aPtr) -> void
				{
					T& ptr = *reinterpret_cast<T*>(aPtr);
					ptr.~T();
				};
		}

		template<typename T>
		constexpr CopyFunction CreateCopyFunction()
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
						new(aDestination)T(source);
					}
				};
		}

		struct MemoryObject
		{
			const DynamicMemoryArenaHandle myHandle;
			const DestructFunction destroy = nullptr;
			const CopyFunction copy = nullptr;
#ifdef _DEBUG
			const std::type_info* myTypeInfo = nullptr;
#endif
		};
	public:

		DynamicMemoryArena(size_t aDefaultCapacity = 0);
		~DynamicMemoryArena();

		DynamicMemoryArena(const DynamicMemoryArena&);
		DynamicMemoryArena(DynamicMemoryArena&&) noexcept;
		DynamicMemoryArena& operator=(const DynamicMemoryArena&);
		DynamicMemoryArena& operator=(DynamicMemoryArena&&) noexcept;

		template<typename T, typename... Args>
		[[nodiscard]] DynamicMemoryArenaHandle Allocate(Args&&... aArgs);

		[[nodiscard]] DynamicMemoryArenaHandle AllocateUnsafe(size_t aSize, InplaceAllocateFunction aInplaceAllocateFunction, DestructFunction aDestructFunction, CopyFunction aCopyFunction);

		template<typename T>
		[[nodiscard]] T& At(DynamicMemoryArenaHandle aHandle);

		template<typename T>
		[[nodiscard]] const T& At(DynamicMemoryArenaHandle aHandle) const;

		[[nodiscard]] size_t GetCapacity() const;
		[[nodiscard]] size_t GetSize() const;
		[[nodiscard]] size_t GetSizeLeft() const;

		[[nodiscard]] std::byte* MemoryAt(DynamicMemoryArenaHandle aHandle);
		[[nodiscard]] const std::byte* MemoryAt(DynamicMemoryArenaHandle aHandle) const;

		void Clear();

	private:

		void AllocateSize(size_t aSize);
		void Reallocate();

	private:
		std::byte* myStartPtr = nullptr;
		size_t myCapacity = 0;
		size_t mySize = 0;

		std::vector<MemoryObject> myObjects;
	};

	template<typename T, typename... Args>
	inline DynamicMemoryArenaHandle DynamicMemoryArena::Allocate(Args&&... aArgs)
	{
		const size_t allocSize = sizeof(T);
		AllocateSize(allocSize);

		const size_t currentSize = GetSize();
		const DynamicMemoryArenaHandle memoryHandle{ currentSize };
		std::byte* currentMemory = myStartPtr + currentSize;
		new(currentMemory)T(std::forward<Args>(aArgs)...);
		mySize += allocSize;
		MemoryObject memoryObject
		{
			.myHandle = memoryHandle,
			.destroy = CreateDestructorInterface<T>(),
			.copy = CreateCopyInterface<T>(),
#ifdef _DEBUG
			.myTypeInfo = &typeid(T)
#endif
		};
		myObjects.push_back(memoryObject);
		return memoryHandle;
	}

	template<typename T>
	inline T& DynamicMemoryArena::At(DynamicMemoryArenaHandle aHandle)
	{
		std::byte* byte = MemoryAt(aHandle);
		return *reinterpret_cast<T*>(byte);
	}

	template<typename T>
	inline const T& DynamicMemoryArena::At(DynamicMemoryArenaHandle aHandle) const
	{
		const std::byte* byte = MemoryAt(aHandle);
		return *reinterpret_cast<const T*>(byte);
	}

	inline std::byte* DynamicMemoryArena::MemoryAt(DynamicMemoryArenaHandle aHandle)
	{
		assert(GetCapacity() > aHandle.myID);
		return myStartPtr + aHandle.myID;
	}

	inline const std::byte* DynamicMemoryArena::MemoryAt(DynamicMemoryArenaHandle aHandle) const
	{
		assert(GetCapacity() > aHandle.myID);
		return myStartPtr + aHandle.myID;
	}

}