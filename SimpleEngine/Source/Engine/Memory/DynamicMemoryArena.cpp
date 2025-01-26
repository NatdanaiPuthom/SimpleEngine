#include "Engine/Precomplied/EnginePch.hpp"
#include "DynamicMemoryArena.hpp"

namespace Simple
{

	DynamicMemoryArena::DynamicMemoryArena(const size_t aDefaultCapacity)
		: myStartPtr(nullptr)
		, myCapacity(aDefaultCapacity)
		, mySize(0)
	{
		if (aDefaultCapacity == 0)
		{
			return;
		}
		myStartPtr = new std::byte[aDefaultCapacity];
	}

	DynamicMemoryArena::~DynamicMemoryArena()
	{
		if (myStartPtr == nullptr)
		{
			return;
		}

		for (size_t i = 0; i < myObjects.size(); ++i)
		{
			MemoryObject& obj = myObjects[i];
			obj.destroy(MemoryAt(obj.myHandle));
		}

		delete[] myStartPtr;
	}

	DynamicMemoryArena::DynamicMemoryArena(const DynamicMemoryArena& aOther)
	{
		myStartPtr = new std::byte[aOther.myCapacity];
		myCapacity = aOther.myCapacity;
		mySize = aOther.mySize;

		for (const MemoryObject& memoryObject : aOther.myObjects)
		{
			std::byte* newAddress = &myStartPtr[memoryObject.myHandle.myID];
			const std::byte* oldAddress = aOther.MemoryAt(memoryObject.myHandle);
			memoryObject.copy(newAddress, oldAddress);

			myObjects.push_back(memoryObject);
		}
	}

	DynamicMemoryArena::DynamicMemoryArena(DynamicMemoryArena&& aOther) noexcept
		: myStartPtr(std::exchange(aOther.myStartPtr, nullptr))
		, myCapacity(std::exchange(aOther.myCapacity, 0))
		, mySize(std::exchange(aOther.mySize, 0))
		, myObjects(std::move(aOther.myObjects))
	{
	}

	DynamicMemoryArena& DynamicMemoryArena::operator=(const DynamicMemoryArena& aOther)
	{
		for (const MemoryObject& memoryObject : myObjects)
		{
			memoryObject.destroy(MemoryAt(memoryObject.myHandle));
		}
		delete[] myStartPtr;

		myStartPtr = new std::byte[aOther.myCapacity];
		myCapacity = aOther.myCapacity;
		mySize = aOther.mySize;

		for (const MemoryObject& memoryObject : aOther.myObjects)
		{
			std::byte* newAddress = &myStartPtr[memoryObject.myHandle.myID];
			const std::byte* oldAddress = aOther.MemoryAt(memoryObject.myHandle);
			memoryObject.copy(newAddress, oldAddress);

			myObjects.push_back(memoryObject);
		}

		return *this;
	}

	DynamicMemoryArena& DynamicMemoryArena::operator=(DynamicMemoryArena&& aOther) noexcept
	{
		myStartPtr = aOther.myStartPtr;
		myCapacity = aOther.myCapacity;
		mySize = aOther.mySize;

		myObjects = std::move(aOther.myObjects);

		aOther.myStartPtr = nullptr;
		aOther.myCapacity = 0;
		aOther.mySize = 0;

		return *this;
	}

	DynamicMemoryArenaHandle DynamicMemoryArena::AllocateUnsafe(const size_t aSize, InplaceAllocateFunction aInplaceAllocateFunction, DestructFunction aDestructorFunction, CopyFunction aCopyFunction)
	{
		assert(aInplaceAllocateFunction);
		assert(aDestructorFunction);
		assert(aCopyFunction);

		AllocateSize(aSize);

		DynamicMemoryArenaHandle memoryHandle{ GetSize() };
		mySize += aSize;
		aInplaceAllocateFunction(MemoryAt(memoryHandle));

		myObjects.push_back(
			MemoryObject
			{
				.myHandle = memoryHandle,
				.destroy = aDestructorFunction,
				.copy = aCopyFunction,
				.myTypeInfo = nullptr
			}
		);

		return memoryHandle;
	}

	size_t DynamicMemoryArena::GetCapacity() const
	{
		return myCapacity;
	}

	size_t DynamicMemoryArena::GetSize() const
	{
		return mySize;
	}

	size_t DynamicMemoryArena::GetSizeLeft() const
	{
		return myCapacity - mySize;
	}

	void DynamicMemoryArena::AllocateSize(const size_t aSize)
	{
		if (myStartPtr == nullptr)
		{
			*this = DynamicMemoryArena(aSize * 2);
		}
		while (aSize > GetSizeLeft())
		{
			Reallocate();
		}
	}

	void DynamicMemoryArena::Reallocate()
	{
		const std::byte* oldStart = myStartPtr;
		const size_t oldCapacity = GetCapacity();
		const size_t newCapacity = oldCapacity * 2;
		const size_t size = GetSize();

		std::byte* newStart = new std::byte[newCapacity];
		for (const MemoryObject& memoryObject : myObjects)
		{
			std::byte* newAddress = &newStart[memoryObject.myHandle.myID];
			const std::byte* oldAddress = MemoryAt(memoryObject.myHandle);
			memoryObject.copy(newAddress, oldAddress);
		}
		delete[] oldStart;

		myStartPtr = newStart;
		myCapacity = newCapacity;
		mySize = size;
	}

	void DynamicMemoryArena::Clear()
	{
		if (myStartPtr == nullptr)
		{
			return;
		}

		for (size_t i = 0; i < myObjects.size(); ++i)
		{
			const MemoryObject& obj = myObjects[i];
			obj.destroy(MemoryAt(obj.myHandle));
		}

		delete[] myStartPtr;

		myStartPtr = nullptr;
		myCapacity = 0;
		mySize = 0;

		myObjects.clear();
	}
}