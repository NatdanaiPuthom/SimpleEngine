#pragma once
#include "../ScriptDefines.h"
#include <memory>
#include <vector>

namespace SCR
{
	using ReleaseFunction = void(*)(void*);
	using CopyFunction = void(*)(void*, const void*);

	template<typename T>
	ReleaseFunction CreateReleaseFunction()
	{
		return [](void* aPtr) -> void
			{
				((T*)aPtr)->~T();
			};
	}

	template<typename T>
	CopyFunction CreateCopyFunction()
	{
		return [](void* aDest, const void* aSource) -> void
			{
				const T& source = *(const T*)aSource;
				new(aDest)T(source);
			};
	}

	struct MemoryObject
	{
		template<typename T>
		MemoryObject(T* aValue)
			: memory(aValue)
			, release(CreateReleaseFunction<T>())
			, copy(CreateCopyFunction<T>())
		{
#ifdef FLY_DEBUG
			typeInfo = &typeid(T);
#endif

		}
		~MemoryObject()
		{
			if (release)
			{
				release(memory);
			}
		}

		MemoryObject(const MemoryObject&) = delete;

		MemoryObject(const MemoryObject& aOther, void* aMemory)
			: memory(aMemory)
			, release(aOther.release)
			, copy(aOther.copy)
		{
			copy(memory, aOther.memory);

#ifdef FLY_DEBUG
			typeInfo = aOther.typeInfo;
#endif
		}

		MemoryObject(MemoryObject&&) noexcept = default;
		MemoryObject& operator=(const MemoryObject&) = delete;
		MemoryObject& operator=(MemoryObject&&) noexcept = default;

		void* memory = nullptr;
		ReleaseFunction release = nullptr;
		CopyFunction copy = nullptr;
#ifdef FLY_DEBUG
		const std::type_info* typeInfo;
#endif
	};

	template<typename T, size_t Size>
	concept MemSizeLessEqual = sizeof(T) <= Size;

	template<size_t Capacity>
	class MemoryBuffer final
	{
	public:

		MemoryBuffer()
			: myBuffer{}
			, myCurrentSize(0)
		{

		}
		~MemoryBuffer()
		{
		}

		MemoryBuffer(const MemoryBuffer& aOther)
			: myBuffer{}
			, myCurrentSize(aOther.myCurrentSize)
		{
			memcpy(myBuffer, aOther.myBuffer, Capacity);
			myMemoryObjects.reserve(aOther.myMemoryObjects.size());
			for (const MemoryObject& memoryObject : aOther.myMemoryObjects)
			{
				size_t ptrDiff = reinterpret_cast<size_t>(memoryObject.memory) - reinterpret_cast<size_t>(&aOther.myBuffer[0]);
				void* newMemory = &myBuffer[0] + ptrDiff;
				myMemoryObjects.emplace_back(memoryObject, newMemory);
			}
		}

		MemoryBuffer(MemoryBuffer&& aOther)
			: myBuffer{}
			, myCurrentSize(aOther.myCurrentSize)
			, myMemoryObjects(std::move(aOther.myMemoryObjects))
		{
			memmove(myBuffer, aOther.myBuffer, Capacity);
			aOther.myCurrentSize = 0;
		}

		MemoryBuffer& operator=(const MemoryBuffer& aOther)
		{
			memcpy(myBuffer, aOther.myBuffer, Capacity);
			myCurrentSize = aOther.myCurrentSize;

			myMemoryObjects.clear();

			for (const MemoryObject& memoryObject : aOther.myMemoryObjects)
			{
				size_t ptrDiff = reinterpret_cast<size_t>(memoryObject.memory) - reinterpret_cast<size_t>(&aOther.myBuffer[0]);
				void* newMemory = &myBuffer[0] + ptrDiff;
				myMemoryObjects.emplace_back(memoryObject, newMemory);
			}

			return *this;
		}

		MemoryBuffer& operator=(MemoryBuffer&& aOther) noexcept
		{
			memmove(myBuffer, aOther.myBuffer, Capacity);
			myCurrentSize = aOther.myCurrentSize;
			myMemoryObjects = std::move(aOther.myMemoryObjects);

			aOther.myCurrentSize = 0;
			return *this;
		}

		template<MemSizeLessEqual<Capacity> T, typename... Args>
		T& Allocate(Args&&... aArgs)
		{
			void* currentMemory = GetCurrentPtr();
			AllocateSize(sizeof(T));
			::new (currentMemory) T(std::forward<Args>(aArgs)...);

			T* value = reinterpret_cast<T*>(currentMemory);

			if constexpr (!std::is_fundamental_v<T>)
			{
				myMemoryObjects.emplace_back(MemoryObject(value));
			}
			return *value;
		}

		void AllocateSize(size_t aSize)
		{
			myCurrentSize += aSize;
		}

		size_t SizeLeft() const
		{
			return Capacity - myCurrentSize;
		}

		void* GetDataPtr()
		{
			return &myBuffer[0];
		}

		const void* GetDataPtr() const
		{
			return &myBuffer[0];
		}

		void* GetCurrentPtr()
		{
			return (&myBuffer[0]) + myCurrentSize;
		}

		const void* GetCurrentPtr() const
		{
			return (&myBuffer[0]) + myCurrentSize;
		}
	private:

		char myBuffer[Capacity];
		size_t myCurrentSize;

		std::vector<MemoryObject> myMemoryObjects;
	};



	template<size_t BufferCapacity>
	class MemoryArena final
	{
		using MemoryBuffer = MemoryBuffer<BufferCapacity>;

	public:

		MemoryArena();
		~MemoryArena();
		MemoryArena(const MemoryArena& aOther);
		MemoryArena(MemoryArena&&) noexcept = default;
		MemoryArena& operator=(const MemoryArena& aOther);
		MemoryArena& operator=(MemoryArena&&) noexcept = default;


		template<MemSizeLessEqual<BufferCapacity> T, typename... Args>
		T& Allocate(Args&&... aArgs);

		void* AllocateSize(size_t aSize);

		void* GetRenewedPointer(const void* aPtr, const MemoryArena& aPrevious) const;

		template<std::integral T>
		T GetID(const void* aPtr) const;

		void Clear();

	private:


		void AllocateNewBuffer();
		MemoryBuffer& GetCurrentBuffer();

	private:

		std::vector<std::unique_ptr<MemoryBuffer>> myMemoryBuffers;
		size_t myCurrentBufferIndex;
	};

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::MemoryArena()
		: myCurrentBufferIndex(0)
	{
		AllocateNewBuffer();
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::~MemoryArena()
	{
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::MemoryArena(const MemoryArena& aOther)
		: myCurrentBufferIndex(aOther.myCurrentBufferIndex)
	{
		for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.myMemoryBuffers)
		{
			myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
		}
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>& MemoryArena<BufferCapacity>::operator=(const MemoryArena<BufferCapacity>& aOther)
	{
		for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.myMemoryBuffers)
		{
			myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
		}
		myCurrentBufferIndex = aOther.myCurrentBufferIndex;
		return *this;
	}

	template<size_t BufferCapacity>
	template<MemSizeLessEqual<BufferCapacity> T, typename ...Args>
	inline T& MemoryArena<BufferCapacity>::Allocate(Args && ...aArgs)
	{
		constexpr size_t allocSize = sizeof(T);
		if (GetCurrentBuffer().SizeLeft() < allocSize)
		{
			AllocateNewBuffer();
		}

		MemoryBuffer& currentBuffer = GetCurrentBuffer();
		T& value = currentBuffer.Allocate<T>(std::forward<Args>(aArgs)...);


		return value;
	}

	template<size_t BufferCapacity>
	inline void* MemoryArena<BufferCapacity>::AllocateSize(size_t aSize)
	{
		if (GetCurrentBuffer().SizeLeft() < aSize)
		{
			AllocateNewBuffer();
		}
		MemoryBuffer& buffer = GetCurrentBuffer();
		void* dataPtr = buffer.GetCurrentPtr();
		buffer.AllocateSize(aSize);
		return dataPtr;
	}

	template<size_t BufferCapacity>
	inline void* MemoryArena<BufferCapacity>::GetRenewedPointer(const void* aPtr, const MemoryArena& aPrevious) const
	{
		for (size_t i = 0; i < aPrevious.myMemoryBuffers.size(); ++i)
		{
			const std::unique_ptr<MemoryBuffer>& buffer = aPrevious.myMemoryBuffers[i];
			const size_t ptrDiff = reinterpret_cast<size_t>(aPtr) - reinterpret_cast<size_t>(buffer->GetDataPtr());
			if (ptrDiff < BufferCapacity)
			{
				return reinterpret_cast<void*>(reinterpret_cast<size_t>(myMemoryBuffers[i]->GetDataPtr()) + ptrDiff);
			}
		}
		return nullptr;
	}

	template<size_t BufferCapacity>
	template<std::integral T>
	inline T MemoryArena<BufferCapacity>::GetID(const void* aPtr) const
	{
		for (size_t i = 0; i < myMemoryBuffers.size(); ++i)
		{
			const std::unique_ptr<MemoryBuffer>& buffer = myMemoryBuffers[i];
			const size_t ptrDiff = reinterpret_cast<size_t>(aPtr) - reinterpret_cast<size_t>(buffer->GetDataPtr());
			if (ptrDiff < BufferCapacity)
			{
				return static_cast<T>(i * BufferCapacity + (reinterpret_cast<size_t>(myMemoryBuffers[i]->GetDataPtr()) + ptrDiff));
			}
		}

		return InvalidID<T>();
	}

	template<size_t BufferCapacity>
	inline void MemoryArena<BufferCapacity>::Clear()
	{
		*this = MemoryArena();
	}

	template<size_t BufferCapacity>
	inline void MemoryArena<BufferCapacity>::AllocateNewBuffer()
	{
		myCurrentBufferIndex = myMemoryBuffers.size();
		myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>());
	}


	template<size_t BufferCapacity>
	inline MemoryBuffer<BufferCapacity>& MemoryArena<BufferCapacity>::GetCurrentBuffer()
	{
		return *myMemoryBuffers.back();
	}



}