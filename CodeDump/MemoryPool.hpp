#pragma once
#include <vector>
#include <typeindex>

//TO-DO(v9.22.3): Deallocate new operator withing MemoryPool adresses such as Allocate<int*> = new int
//NOTE(v9.24.5): Maybe not needed

using Deleter = void(*)(void*);
using DataType = std::type_index;
using MemoryAdress = size_t;

template<typename T>
inline Deleter DestructorPointer()
{
	return [](void* aPointer)
		{
			static_cast<T*>(aPointer)->~T();
		};
}

struct MemoryObject
{
	MemoryAdress id;
	DataType type;
	Deleter deleter;
};

class MemoryPool
{
public:
	inline MemoryPool(size_t aDefaultSize = 100)
	{
		myStartMemoryAdress = new char[aDefaultSize];
		myEndMemoryAdress = myStartMemoryAdress + sizeof(char) * aDefaultSize;
		myCurrentMemoryAdress = myStartMemoryAdress;
	}

	inline ~MemoryPool()
	{
		for (int i = static_cast<int>(myObjects.size()) - 1; i >= 0; --i)
		{
			MemoryObject& object = myObjects[i];
			object.deleter(GetAdress(object.id));
		}

		delete[] myStartMemoryAdress;
	}

	MemoryPool(const MemoryPool&) = delete;
	MemoryPool(MemoryPool&&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
	MemoryPool& operator=(MemoryPool&&) = delete;

	template<typename T>
	inline size_t Allocate();

	template<typename T>
	inline T& GetValue(const size_t aIndex);

	template<typename T>
	inline const T& GetValue(const size_t aIndex) const;

	template<typename T>
	inline T GetValue(const size_t aIndex) const;

	inline size_t GetSize() const;
	inline size_t GetCapacity() const;
	inline size_t GetAvaliableMemorySize() const;
	inline const MemoryObject& GetAllocatedObject(const size_t aIndex) const;

private:
	inline char* GetAdress(const MemoryAdress aID) const;
	inline void Reallocate();
private:
	char* myStartMemoryAdress;
	char* myEndMemoryAdress;
	char* myCurrentMemoryAdress;

	std::vector<MemoryObject> myObjects;
};

template<typename T>
inline size_t MemoryPool::Allocate()
{
	constexpr size_t objectSize = sizeof(T);

	while (objectSize > GetAvaliableMemorySize())
	{
		Reallocate();
	}

	const MemoryAdress id = GetSize();

	myCurrentMemoryAdress += objectSize;
	myObjects.emplace_back(id, typeid(T), DestructorPointer<T>());

	return myObjects.size() - 1;
}

inline char* MemoryPool::GetAdress(const MemoryAdress aID) const
{
	return myStartMemoryAdress + aID;
}

inline size_t MemoryPool::GetCapacity() const
{
	return myEndMemoryAdress - myStartMemoryAdress;
}

inline void MemoryPool::Reallocate()
{
	char* oldMemoryArray = myStartMemoryAdress;

	const size_t oldMemoryCapacity = GetCapacity();
	const size_t currentMemorySize = GetSize();
	const size_t newMemoryCapacity = oldMemoryCapacity * 2;

	myStartMemoryAdress = new char[newMemoryCapacity];
	std::memcpy(myStartMemoryAdress, oldMemoryArray, oldMemoryCapacity);
	delete[] oldMemoryArray;

	myCurrentMemoryAdress = myStartMemoryAdress + currentMemorySize;
	myEndMemoryAdress = myStartMemoryAdress + newMemoryCapacity;
}

template<typename T>
inline T& MemoryPool::GetValue(const size_t aIndex)
{
	if (typeid(T) != myObjects[aIndex].type)
	{
		assert(false && "Incorrect data type");
	}

	return (T&)*GetAdress(myObjects[aIndex].id);
}

template<typename T>
inline const T& MemoryPool::GetValue(const size_t aIndex) const
{
	if (typeid(T) != myObjects[aIndex].type)
	{
		assert(false && "Incorrect data type");
	}

	return (const T&)*GetAdress(myObjects[aIndex].id);
}

template<typename T>
inline T MemoryPool::GetValue(const size_t aIndex) const
{
	if (typeid(T) != myObjects[aIndex].type)
	{
		assert(false && "Incorrect data type");
	}

	T value = {};
	std::memcpy(&value, GetAdress(myObjects[aIndex].id), sizeof(T));
	return value;
}

inline size_t MemoryPool::GetSize() const
{
	return myCurrentMemoryAdress - myStartMemoryAdress;
}

inline size_t MemoryPool::GetAvaliableMemorySize() const
{
	return myEndMemoryAdress - myCurrentMemoryAdress;
}

inline const MemoryObject& MemoryPool::GetAllocatedObject(const size_t aIndex) const
{
	return myObjects[aIndex];
}