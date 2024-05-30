#include "ScriptMemoryPool.h"
#include <cassert>

namespace SCR
{

	MemoryPool::MemoryPool(size_t aDefaultSize)
		: myStartMemory(nullptr)
		, myEndMemory(nullptr)
		, myCurrentMemory(nullptr)
	{
		if (aDefaultSize == 0)
		{

			return;
		}
		myStartMemory = new Byte[aDefaultSize];
		myEndMemory = myStartMemory + sizeof(Byte) * aDefaultSize;
		myCurrentMemory = myStartMemory;
	}

	MemoryPool::~MemoryPool()
	{
		if (myStartMemory == nullptr)
		{
			return;
		}

		for (int i = 0; i < myObjects.size(); ++i)
		{
			MemoryObject& obj = myObjects[i];
			obj.destroy(MemoryAt(obj.id));
		}

		delete[] myStartMemory;

		myStartMemory = nullptr;
		myEndMemory = nullptr;
		myCurrentMemory = nullptr;

		myObjects.~vector();
	}

	MemoryPool::MemoryPool(const MemoryPool& aOther)
	{
		size_t size = aOther.AllocSize();
		myStartMemory = new Byte[size];
		myEndMemory = myStartMemory + size;
		myCurrentMemory = myStartMemory;

		for (const MemoryObject& memoryObject : aOther.myObjects)
		{
			Byte* newAddress = &myStartMemory[memoryObject.id];
			const Byte* oldAddress = aOther.MemoryAt(memoryObject.id);
			memoryObject.copy(newAddress, oldAddress);

			myObjects.push_back(memoryObject);
		}
	}

	MemoryPool::MemoryPool(MemoryPool&& aOther) noexcept
		: myStartMemory(aOther.myStartMemory)
		, myEndMemory(aOther.myEndMemory)
		, myCurrentMemory(aOther.myCurrentMemory)
		, myObjects(std::move(aOther.myObjects))
	{

		aOther.myStartMemory = nullptr;
		aOther.myCurrentMemory = nullptr;
		aOther.myEndMemory = nullptr;

		aOther.myObjects.clear();
	}

	MemoryPool& MemoryPool::operator=(const MemoryPool& aOther)
	{
		size_t size = aOther.AllocSize();
		myStartMemory = new Byte[size];
		myEndMemory = myStartMemory + size;
		myCurrentMemory = myStartMemory;

		for (const MemoryObject& memoryObject : aOther.myObjects)
		{
			Byte* newAddress = &myStartMemory[memoryObject.id];
			const Byte* oldAddress = aOther.MemoryAt(memoryObject.id);
			memoryObject.copy(newAddress, oldAddress);

			myObjects.push_back(memoryObject);
		}

		return *this;
	}

	MemoryPool& MemoryPool::operator=(MemoryPool&& aOther) noexcept
	{
		myStartMemory = aOther.myStartMemory;
		myEndMemory = aOther.myEndMemory;
		myCurrentMemory = aOther.myCurrentMemory;

		myObjects = std::move(aOther.myObjects);

		aOther.myStartMemory = nullptr;
		aOther.myCurrentMemory = nullptr;
		aOther.myEndMemory = nullptr;

		aOther.myObjects.clear();

		return *this;
	}

	size_t MemoryPool::AllocSize() const
	{
		return myEndMemory - myStartMemory;
	}

	size_t MemoryPool::CurrentSize() const
	{
		return myCurrentMemory - myStartMemory;
	}

	size_t MemoryPool::SizeLeft() const
	{
		return myEndMemory - myCurrentMemory;
	}

	void MemoryPool::Reallocate()
	{
		const Byte* oldStart = myStartMemory;
		const size_t oldSize = AllocSize();
		const size_t newSize = oldSize * 2;
		const size_t currentSize = CurrentSize();

		myStartMemory = new Byte[newSize];
		std::memcpy(myStartMemory, oldStart, oldSize);
		delete[] oldStart;

		myCurrentMemory = myStartMemory + currentSize;
		myEndMemory = myStartMemory + newSize;
	}

	void MemoryPool::ReallocateDeepCopy()
	{
		const Byte* oldStart = myStartMemory;
		const size_t oldSize = AllocSize();
		const size_t newSize = oldSize * 2;
		const size_t currentSize = CurrentSize();

		Byte* newStart = new Byte[newSize];
		for (const MemoryObject& memoryObject : myObjects)
		{
			Byte* newAddress = &newStart[memoryObject.id];
			const Byte* oldAddress = MemoryAt(memoryObject.id);
			//memoryObject.construct(newAddress);
			memoryObject.copy(newAddress, oldAddress);
		}
		//std::memcpy(newStart, oldStart, oldSize);
		delete[] oldStart;

		myStartMemory = newStart;
		myCurrentMemory = myStartMemory + currentSize;
		myEndMemory = myStartMemory + newSize;
	}

	void MemoryPool::Clear()
	{
		if (myStartMemory == nullptr)
		{
			return;
		}

		for (int i = 0; i < myObjects.size(); ++i)
		{
			MemoryObject& obj = myObjects[i];
			obj.destroy(MemoryAt(obj.id));
		}

		delete[] myStartMemory;

		myStartMemory = nullptr;
		myEndMemory = nullptr;
		myCurrentMemory = nullptr;

		myObjects.clear();
	}
}