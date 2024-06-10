#pragma once
#include "../ScriptDefines.h"
#include <list>

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

		template<MemSizeLessEqual<Capacity> T>
		T& Allocate(const T& aDefaultValue = T())
		{
			void* currentMemory = (&myBuffer[0]) + myCurrentSize;
			new (currentMemory) T(aDefaultValue);

			myCurrentSize += sizeof(T);

			T* value = reinterpret_cast<T*>(currentMemory);

			if constexpr (!std::is_fundamental_v<T>)
			{
				myMemoryObjects.emplace_back(value);
			}
			return *value;
		}

		template<MemSizeLessEqual<Capacity> T, typename... Args>
		T& EmplaceAllocate(Args&&... args)
		{
			void* currentMemory = (&myBuffer[0]) + myCurrentSize;
			new (currentMemory) T(std::forward<Args>(args)...);

			myCurrentSize += sizeof(T);

			T* value = reinterpret_cast<T*>(currentMemory);

			if constexpr (!std::is_fundamental_v<T>)
			{
				myMemoryObjects.emplace_back(value);
			}
			return *value;
		}

		size_t SizeLeft() const
		{
			return Capacity - myCurrentSize;
		}

		void* Data()
		{
			return &myBuffer[0];
		}

		const void* Data() const
		{
			return &myBuffer[0];
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
		MemoryArena()
			: myCurrentBufferIndex(0)
		{
			AllocateNewBuffer();
		}
		~MemoryArena()
		{
			std::cout << "Destroyed Arena" << std::endl;
		}

		MemoryArena(const MemoryArena& aOther)
			: myCurrentBufferIndex(aOther.myCurrentBufferIndex)
		{
			for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.myMemoryBuffers)
			{
				myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
			}
		}

		MemoryArena(MemoryArena&&) noexcept = default;

		MemoryArena& operator=(const MemoryArena& aOther)
		{
			for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.myMemoryBuffers)
			{
				myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
			}
			myCurrentBufferIndex = aOther.myCurrentBufferIndex;
			return *this;
		}


		MemoryArena& operator=(MemoryArena&&) = default;


		template<MemSizeLessEqual<BufferCapacity> T, typename... Args>
		T& Allocate(const T& aDefaultValue = T())
		{
			constexpr size_t allocSize = sizeof(T);
			if (GetCurrentBuffer().SizeLeft() < allocSize)
			{
				AllocateNewBuffer();
			}

			MemoryBuffer& currentBuffer = GetCurrentBuffer();
			T& value = currentBuffer.Allocate(aDefaultValue);


			return value;
		}

		void* GetRenewedPointer(void* aPtr, const MemoryArena& aPrevious) const
		{
			for (size_t i = 0; const std::unique_ptr<MemoryBuffer>&buffer : aPrevious.myMemoryBuffers)
			{
				size_t ptrDiff = reinterpret_cast<size_t>(aPtr) - reinterpret_cast<size_t>(buffer->Data());
				if (ptrDiff < BufferCapacity)
				{
					return reinterpret_cast<void*>(reinterpret_cast<size_t>(myMemoryBuffers[i]->Data()) + ptrDiff);

				}

				i++;
			}

			return nullptr;
		}

	private:


		void AllocateNewBuffer()
		{
			myCurrentBufferIndex = myMemoryBuffers.size();
			myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>());
		}

		MemoryBuffer& GetCurrentBuffer()
		{
			return *myMemoryBuffers.back();
		}

	private:

		std::vector<std::unique_ptr<MemoryBuffer>> myMemoryBuffers;
		size_t myCurrentBufferIndex;
	};
}