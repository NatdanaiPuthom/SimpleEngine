#include "FlyMemoryArena.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include <cassert>

namespace FLY_NAMESPACE
{

	MemoryPool::MemoryPool(const size_t aDefaultCapacity)
		: mStartPtr(nullptr)
		, mCapacity(aDefaultCapacity)
		, mSize(0)
	{
		if (aDefaultCapacity == 0)
		{
			return;
		}
		mStartPtr = new Byte[aDefaultCapacity];
	}

	MemoryPool::~MemoryPool()
	{
		if (mStartPtr == nullptr)
		{
			return;
		}

		for (int i = 0; i < mObjects.size(); ++i)
		{
			MemoryObject& obj = mObjects[i];
			obj.destroy(MemoryAt(obj.id));
		}

		delete[] mStartPtr;
	}

	MemoryPool::MemoryPool(const MemoryPool& aOther)
	{
		mStartPtr = new Byte[aOther.mCapacity];
		mCapacity = aOther.mCapacity;
		mSize = aOther.mSize;

		for (const MemoryObject& memoryObject : aOther.mObjects)
		{
			Byte* newAddress = &mStartPtr[memoryObject.id];
			const Byte* oldAddress = aOther.MemoryAt(memoryObject.id);
			memoryObject.copy(newAddress, oldAddress);

			mObjects.push_back(memoryObject);
		}
	}

	MemoryPool::MemoryPool(MemoryPool&& aOther) noexcept
		: mStartPtr(aOther.mStartPtr)
		, mCapacity(aOther.mCapacity)
		, mSize(aOther.mSize)
		, mObjects(std::move(aOther.mObjects))
	{
		aOther.mStartPtr = nullptr;
		aOther.mCapacity = 0;
		aOther.mSize = 0;
	}

	MemoryPool& MemoryPool::operator=(const MemoryPool& aOther)
	{
		for (const MemoryObject& memoryObject : mObjects)
		{
			memoryObject.destroy(MemoryAt(memoryObject.id));
		}
		delete[] mStartPtr;

		mStartPtr = new Byte[aOther.mCapacity];
		mCapacity = aOther.mCapacity;
		mSize = aOther.mSize;

		for (const MemoryObject& memoryObject : aOther.mObjects)
		{
			Byte* newAddress = &mStartPtr[memoryObject.id];
			const Byte* oldAddress = aOther.MemoryAt(memoryObject.id);
			memoryObject.copy(newAddress, oldAddress);

			mObjects.push_back(memoryObject);
		}

		return *this;
	}

	MemoryPool& MemoryPool::operator=(MemoryPool&& aOther) noexcept
	{
		mStartPtr = aOther.mStartPtr;
		mCapacity = aOther.mCapacity;
		mSize = aOther.mSize;

		mObjects = std::move(aOther.mObjects);

		aOther.mStartPtr = nullptr;
		aOther.mCapacity = 0;
		aOther.mSize = 0;

		return *this;
	}

	size_t MemoryPool::GetCapacity() const
	{
		return mCapacity;
	}

	size_t MemoryPool::GetSize() const
	{
		return mSize;
	}

	size_t MemoryPool::GetSizeLeft() const
	{
		return mCapacity - mSize;
	}

	void MemoryPool::Reallocate()
	{
		const Byte* oldStart = mStartPtr;
		const size_t oldCapacity = GetCapacity();
		const size_t newCapacity = oldCapacity * 2;
		const size_t size = GetSize();

		Byte* newStart = new Byte[newCapacity];
		for (const MemoryObject& memoryObject : mObjects)
		{
			Byte* newAddress = &newStart[memoryObject.id];
			const Byte* oldAddress = MemoryAt(memoryObject.id);
			memoryObject.copy(newAddress, oldAddress);
		}
		delete[] oldStart;

		mStartPtr = newStart;
		mCapacity = newCapacity;
		mSize = size;
	}

	void MemoryPool::Clear()
	{
		if (mStartPtr == nullptr)
		{
			return;
		}

		for (size_t i = 0; i < mObjects.size(); ++i)
		{
			const MemoryObject& obj = mObjects[i];
			obj.destroy(MemoryAt(obj.id));
		}

		delete[] mStartPtr;

		mStartPtr = nullptr;
		mCapacity = 0;
		mSize = 0;

		mObjects.clear();
	}

	static DataType* GetDataType(DataTypeID aDataTypeID)
	{
		return Internal::GetDataTypeManager().Find(aDataTypeID);
	}

	MemoryObject::MemoryObject(void* const aDataPtr, const DataTypeID aDataTypeID)
		: mDataPtr(aDataPtr)
	{
		DataType* dataType = GetDataType(aDataTypeID);
		assert(dataType);

		release = dataType->GetInterface().GetReleaseFunction().GetPtr();
		copy = dataType->GetInterface().GetCopyFunction().GetPtr();

#ifdef FLY_DEBUG
		mTypeInfo = dataType->GetTypeInfo();
#endif
	}

	MemoryObject::~MemoryObject()
	{
		if (release)
		{
			release(mDataPtr);
		}
	}

	MemoryObject::MemoryObject(const MemoryObject& aOther, void* const aDataPtr)
		: mDataPtr(aDataPtr)
		, release(aOther.release)
		, copy(aOther.copy)
	{
		copy(mDataPtr, aOther.mDataPtr);

#ifdef FLY_DEBUG
		mTypeInfo = aOther.mTypeInfo;
#endif
	}

	MemoryObject::MemoryObject(MemoryObject&& aOther) noexcept
		: mDataPtr(std::exchange(aOther.mDataPtr, nullptr))
		, release(std::exchange(aOther.release, nullptr))
		, copy(std::exchange(aOther.copy, nullptr))
#ifdef FLY_DEBUG
		, mTypeInfo(std::exchange(aOther.mTypeInfo, nullptr))
#endif
	{
	}
}