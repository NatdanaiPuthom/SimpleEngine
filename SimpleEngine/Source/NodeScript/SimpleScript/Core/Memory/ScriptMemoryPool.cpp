#include "ScriptMemoryPool.h"
#include <cassert>

namespace SCR
{

	MemoryPool::MemoryPool(size_t aDefaultSize)
	{
		assert(aDefaultSize != 0);
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
			Byte* oldAddress = MemoryAt(memoryObject.id);
			//memoryObject.construct(newAddress);
			memoryObject.copy(newAddress, oldAddress);
		}
		//std::memcpy(newStart, oldStart, oldSize);
		delete[] oldStart;

		myStartMemory = newStart;
		myCurrentMemory = myStartMemory + currentSize;
		myEndMemory = myStartMemory + newSize;
	}

	Byte* MemoryPool::MemoryAt(MemoryPoolID anID)
	{
		return myStartMemory + anID;
	}

	const Byte* MemoryPool::MemoryAt(MemoryPoolID anID) const
	{
		return myStartMemory + anID;
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