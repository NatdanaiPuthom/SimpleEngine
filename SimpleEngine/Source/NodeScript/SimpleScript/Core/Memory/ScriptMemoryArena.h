#pragma once
#include "../ScriptDefines.h"
#include <list>

namespace SCR
{

	template<typename T, size_t Size>
	concept MemSizeLessEqual = sizeof(T) <= Size;

	template<size_t Capacity>
	class MemoryBuffer final
	{
	public:

		MemoryBuffer()
			: myCurrentSize(0)
		{

		}
		~MemoryBuffer()
		{
		}

		template<MemSizeLessEqual<Capacity> T>
		T& Allocate(const T& aDefaultValue = T())
		{
			void* currentMemory = (&myBuffer[0]) + myCurrentSize;
			new (currentMemory) T(aDefaultValue);

			myCurrentSize += sizeof(T);

			return *reinterpret_cast<T*>(currentMemory);
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
	};

	using CopyFunction = void(*)(void*, const void*);

	template<typename T>
	CopyFunction CreateCopyFunction()
	{
		return [](void* aDest, const void* aSource) -> void
			{
				T& dest = *(T*)aDest;
				const T& source = *(const T*)aSource;

				dest = source;
				source;
				dest;

			};
	}

	template<size_t BufferCapacity>
	class MemoryArena final
	{
		struct MemoryObject
		{
			template<typename T>
			MemoryObject(T* aValue, size_t aBufferIndex)
				: memory(aValue)
				, bufferIndex(aBufferIndex)
				, release([](void* aPtr) -> void { ((T*)aPtr)->~T(); })
				, copy(CreateCopyFunction<T>())
			{
				std::cout << "Constructor" << std::endl;
#ifdef FLY_DEBUG
				typeInfo = &typeid(T);
#endif

			}
			~MemoryObject()
			{
				std::cout << "Destructor" << std::endl;
				if (release)
				{
					release(memory);
				}
			}

			MemoryObject(const MemoryObject&) = delete;

			MemoryObject(const MemoryObject& aOther, void* aMemory)
				: memory(aMemory)
				, bufferIndex(aOther.bufferIndex)
				, release(aOther.release)
				, copy(aOther.copy)

			{
				copy(memory, aOther.memory);

#ifdef FLY_DEBUG
				typeInfo = aOther.typeInfo;
#endif
			}

			MemoryObject(MemoryObject&& aOther) noexcept
				: release(aOther.release)
				, copy(aOther.copy)
				, memory(aOther.memory)
			{
				std::cout << "Move constructor" << std::endl;
#ifdef FLY_DEBUG
				typeInfo = aOther.typeInfo;
#endif
				aOther.release = nullptr;
				aOther.memory = nullptr;
				aOther.copy = nullptr;
				aOther.typeInfo = nullptr;
			}

			MemoryObject& operator=(const MemoryObject& aOther)
			{
				std::cout << "Copy assign" << std::endl;

				aOther;
				return *this;
			}

			MemoryObject& operator=(MemoryObject&& aOther)
			{
				std::cout << "Move assign" << std::endl;

				aOther;
				return *this;
			}

			/*void Assign(const void* aSource)
			{
				memory = aSource;
			}*/
			void* memory = nullptr;
			size_t bufferIndex = 0;
			void(*release)(void*) = nullptr;
			CopyFunction copy = nullptr;
#ifdef FLY_DEBUG
			const std::type_info* typeInfo;
#endif
		};
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

			myMemoryObjects.reserve(aOther.myMemoryObjects.size());
			for (/*size_t i = 0; */const MemoryObject& memoryObject : aOther.myMemoryObjects)
			{
				size_t ptrDiff = reinterpret_cast<size_t>(memoryObject.memory) - reinterpret_cast<size_t>(aOther.myMemoryBuffers[memoryObject.bufferIndex]->Data());
				void* newMemory = reinterpret_cast<void*>(reinterpret_cast<size_t>(myMemoryBuffers[memoryObject.bufferIndex]->Data()) + ptrDiff);
				myMemoryObjects.emplace_back(memoryObject, newMemory);
				//memoryObject.copy(memoryObject.memory, aOther.myMemoryObjects[i].memory);
				//i++;
			}
		}

		MemoryArena(MemoryArena&& aOther)
			: myMemoryBuffers(std::move(aOther.myMemoryBuffers))
			, myCurrentBufferIndex(aOther.myCurrentBufferIndex)
			, myMemoryObjects(std::move(aOther.myMemoryObjects))
		{
			aOther.myCurrentBufferIndex = 0;
		}

		MemoryArena& operator=(const MemoryArena& aOther)
		{

			for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.myMemoryBuffers)
			{
				myMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
			}
			myCurrentBufferIndex = aOther.myCurrentBufferIndex;

			myMemoryObjects = aOther.myMemoryObjects;
			return *this;
		}


		MemoryArena& operator=(MemoryArena&& aOther)
		{
			myMemoryBuffers = std::move(aOther.myMemoryBuffers);
			myCurrentBufferIndex = aOther.myCurrentBufferIndex;
			myMemoryObjects = std::move(aOther.myMemoryObjects);

			aOther.myCurrentBufferIndex = 0;

			return *this;
		}


		template<MemSizeLessEqual<BufferCapacity> T>
		T& Allocate(const T& aDefaultValue = T())
		{
			constexpr size_t allocSize = sizeof(T);
			if (GetCurrentBuffer().SizeLeft() < allocSize)
			{
				AllocateNewBuffer();
			}

			MemoryBuffer& currentBuffer = GetCurrentBuffer();
			T& value = currentBuffer.Allocate(aDefaultValue);

			myMemoryObjects.emplace_back(&value, myCurrentBufferIndex);

			return value;
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
		std::vector<MemoryObject> myMemoryObjects;
	};
}