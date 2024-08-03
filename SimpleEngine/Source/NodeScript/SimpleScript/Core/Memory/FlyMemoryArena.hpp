#pragma once
#include "../FlyDefines.hpp"
#include <memory>
#include <vector>

namespace FLY_NAMESPACE
{

	using Byte = unsigned char;
	using MemoryPoolID = size_t;

	class MemoryPool final
	{

		using ConstructorInterface = void(*)(void*);
		using DestructorInterface = void(*)(void*);
		using CopyInterface = void(*)(void*, const void*);

		template<typename T>
		inline ConstructorInterface CreateConstructorInterface()
		{
			return [](void* aPtr) -> void
				{
					new(aPtr)T();
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
						new(aDestination)T(source);
					}
				};
		}

		struct MemoryObject
		{
			const MemoryPoolID id;
			const DestructorInterface destroy;
			const CopyInterface copy;
#ifdef FLY_DEBUG
			const std::type_info* mTypeInfo;
#endif
		};
	public:

		MemoryPool(size_t aDefaultCapacity = 0);
		~MemoryPool();

		MemoryPool(const MemoryPool&);
		MemoryPool(MemoryPool&&) noexcept;
		MemoryPool& operator=(const MemoryPool&);
		MemoryPool& operator=(MemoryPool&&) noexcept;

		template<typename T, typename... Args>
		MemoryPoolID Allocate(Args&&... aArgs);

		template<typename T>
		T& At(MemoryPoolID anID);

		template<typename T>
		const T& At(MemoryPoolID anID) const;

		size_t GetCapacity() const;
		size_t GetSize() const;
		size_t GetSizeLeft() const;

		Byte* MemoryAt(MemoryPoolID anID);
		const Byte* MemoryAt(MemoryPoolID anID) const;

		void Clear();

	private:
		void Reallocate();
	private:
		Byte* mStartPtr;
		size_t mCapacity;
		size_t mSize;

		std::vector<MemoryObject> mObjects;
	};

	template<typename T, typename... Args>
	inline MemoryPoolID MemoryPool::Allocate(Args&&... aArgs)
	{
		constexpr size_t newAllocSize = sizeof(T);
		if (mStartPtr == nullptr)
		{
			*this = MemoryPool(newAllocSize * 2);
		}
		while (newAllocSize > GetSizeLeft())
		{
			Reallocate();
		}

		const MemoryPoolID id = GetSize();
		Byte* currentMemory = mStartPtr + id;
		new(currentMemory)T(std::forward<Args>(aArgs)...);
		mSize += newAllocSize;
		MemoryObject memoryObject
		{
			.id = id,
			.destroy = CreateDestructorInterface<T>(),
			.copy = CreateCopyInterface<T>(),
#ifdef FLY_DEBUG
			.mTypeInfo = &typeid(T)
#endif
		};
		mObjects.push_back(memoryObject);
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

	inline Byte* MemoryPool::MemoryAt(MemoryPoolID anID)
	{
		return mStartPtr + anID;
	}

	inline const Byte* MemoryPool::MemoryAt(MemoryPoolID anID) const
	{
		return mStartPtr + anID;
	}


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

	struct MemoryObject final
	{
		template<typename T>
		MemoryObject(T* aValue)
			: mDataPtr(aValue)
			, release(CreateReleaseFunction<T>())
			, copy(CreateCopyFunction<T>())
		{
#ifdef FLY_DEBUG
			mTypeInfo = &typeid(T);
#endif
		}

		MemoryObject(void* aDataPtr, DataTypeID aDataTypeID);

		~MemoryObject();

		MemoryObject(const MemoryObject&) = delete;

		MemoryObject(const MemoryObject& aOther, void* aMemory);

		MemoryObject(MemoryObject&&) noexcept;
		MemoryObject& operator=(const MemoryObject&) = delete;
		MemoryObject& operator=(MemoryObject&&) noexcept = default;

	public:
		void* mDataPtr = nullptr;
		ReleaseFunction release = nullptr;
		CopyFunction copy = nullptr;
#ifdef FLY_DEBUG
		const std::type_info* mTypeInfo;
#endif
	};

	inline size_t GetPointerDiff(const void* const aPtr1, const void* const aPtr2)
	{
		return static_cast<const Byte*>(aPtr1) - static_cast<const Byte*>(aPtr2);
	}

	template<typename T, size_t Size>
	concept MemSizeLessEqual = sizeof(T) <= Size;

	template<size_t Capacity>
	class MemoryBuffer final
	{
	public:

		MemoryBuffer()
			: mBuffer{}
			, mSize(0)
		{

		}
		~MemoryBuffer()
		{
		}

		MemoryBuffer(const MemoryBuffer& aOther)
			: mBuffer{}
			, mSize(aOther.mSize)
		{
			std::memcpy(mBuffer, aOther.mBuffer, Capacity);
			mMemoryObjects.reserve(aOther.mMemoryObjects.size());
			for (const MemoryObject& memoryObject : aOther.mMemoryObjects)
			{
				const size_t ptrDiff = GetPointerDiff(memoryObject.mDataPtr, &aOther.mBuffer[0]);
				void* newMemory = &mBuffer[0] + ptrDiff;
				mMemoryObjects.emplace_back(memoryObject, newMemory);
			}
		}

		MemoryBuffer(MemoryBuffer&& aOther)
			: mBuffer{}
			, mSize(aOther.mSize)
			, mMemoryObjects(std::move(aOther.mMemoryObjects))
		{
			std::memmove(mBuffer, aOther.mBuffer, Capacity);
			aOther.mSize = 0;
		}

		MemoryBuffer& operator=(const MemoryBuffer& aOther)
		{
			std::memcpy(mBuffer, aOther.mBuffer, Capacity);
			mSize = aOther.mSize;

			mMemoryObjects.clear();

			for (const MemoryObject& memoryObject : aOther.mMemoryObjects)
			{
				const size_t ptrDiff = GetPointerDiff(memoryObject.mDataPtr, &aOther.mBuffer[0]);
				void* newMemory = &mBuffer[0] + ptrDiff;
				mMemoryObjects.emplace_back(memoryObject, newMemory);
			}

			return *this;
		}

		MemoryBuffer& operator=(MemoryBuffer&& aOther) noexcept
		{
			std::memmove(mBuffer, aOther.mBuffer, Capacity);
			mSize = aOther.mSize;
			mMemoryObjects = std::move(aOther.mMemoryObjects);

			aOther.mSize = 0;
			return *this;
		}

		template<MemSizeLessEqual<Capacity> T, typename... Args>
		T& Allocate(Args&&... aArgs)
		{
			void* currentMemory = GetCurrentPtr();
			AllocateSize(sizeof(T));

			T* value = reinterpret_cast<T*>(currentMemory);

			std::construct_at(value, std::forward<Args>(aArgs)...);

			if constexpr (!std::is_fundamental_v<T>)
			{
				mMemoryObjects.emplace_back(MemoryObject(value));
			}
			return *value;
		}


		void AllocateSize(size_t aSize)
		{
			mSize += aSize;
		}

		void RegisterMemoryObject(void* aDataPtr, DataTypeID aDataTypeID)
		{
			assert(GetPointerDiff(aDataPtr, GetDataPtr()) < Capacity);

			mMemoryObjects.emplace_back(MemoryObject(aDataPtr, aDataTypeID));
		}

		[[nodiscard]] size_t GetSizeLeft() const
		{
			return Capacity - mSize;
		}

		[[nodiscard]] void* GetDataPtr()
		{
			return &mBuffer[0];
		}

		[[nodiscard]] const void* GetDataPtr() const
		{
			return &mBuffer[0];
		}

		[[nodiscard]] void* GetCurrentPtr()
		{
			return (&mBuffer[0]) + mSize;
		}

		[[nodiscard]] const void* GetCurrentPtr() const
		{
			return (&mBuffer[0]) + mSize;
		}
	private:

		char mBuffer[Capacity];
		size_t mSize;

		std::vector<MemoryObject> mMemoryObjects;
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

		void RegisterMemoryObject(void* aDataPtr, DataTypeID aDataTypeID);

		[[nodiscard]] void* GetRenewedPointer(const void* aDataPtr, const MemoryArena& aPrevious) const;

		template<std::integral T>
		[[nodiscard]] T GetID(const void* aPtr) const;

		void Clear();

	private:


		void AllocateNewBuffer();
		[[nodiscard]] MemoryBuffer& GetCurrentBuffer();
		[[nodiscard]] MemoryBuffer& GetMemoryBuffer(const void* aDataPtr);

	private:

		std::vector<std::unique_ptr<MemoryBuffer>> mMemoryBuffers;
		size_t mCurrentBufferIndex;
	};

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::MemoryArena()
		: mCurrentBufferIndex(0)
	{
		AllocateNewBuffer();
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::~MemoryArena()
	{
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>::MemoryArena(const MemoryArena& aOther)
		: mCurrentBufferIndex(aOther.mCurrentBufferIndex)
	{
		for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.mMemoryBuffers)
		{
			mMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
		}
	}

	template<size_t BufferCapacity>
	inline MemoryArena<BufferCapacity>& MemoryArena<BufferCapacity>::operator=(const MemoryArena<BufferCapacity>& aOther)
	{
		for (const std::unique_ptr<MemoryBuffer>& buffer : aOther.mMemoryBuffers)
		{
			mMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>(*buffer));
		}
		mCurrentBufferIndex = aOther.mCurrentBufferIndex;
		return *this;
	}

	template<size_t BufferCapacity>
	template<MemSizeLessEqual<BufferCapacity> T, typename ...Args>
	inline T& MemoryArena<BufferCapacity>::Allocate(Args && ...aArgs)
	{
		constexpr size_t allocSize = sizeof(T);
		if (GetCurrentBuffer().GetSizeLeft() < allocSize)
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
		if (aSize > BufferCapacity)
		{
			throw std::runtime_error("Trying to allocate size that is larger than the set buffer capacity.");
		}
		if (GetCurrentBuffer().GetSizeLeft() < aSize)
		{
			AllocateNewBuffer();
		}
		MemoryBuffer& buffer = GetCurrentBuffer();
		void* dataPtr = buffer.GetCurrentPtr();
		buffer.AllocateSize(aSize);
		return dataPtr;
	}

	template<size_t BufferCapacity>
	inline void MemoryArena<BufferCapacity>::RegisterMemoryObject(void* const aDataPtr, const DataTypeID aDataTypeID)
	{
		MemoryBuffer& buffer = GetCurrentBuffer();
		buffer.RegisterMemoryObject(aDataPtr, aDataTypeID);
	}

	template<size_t BufferCapacity>
	inline void* MemoryArena<BufferCapacity>::GetRenewedPointer(const void* aDataPtr, const MemoryArena& aPrevious) const
	{
		for (size_t i = 0; i < aPrevious.mMemoryBuffers.size(); ++i)
		{
			const std::unique_ptr<MemoryBuffer>& buffer = aPrevious.mMemoryBuffers[i];
			const size_t ptrDiff = GetPointerDiff(aDataPtr, buffer->GetDataPtr());
			if (ptrDiff < BufferCapacity)
			{
				return reinterpret_cast<void*>(reinterpret_cast<size_t>(mMemoryBuffers[i]->GetDataPtr()) + ptrDiff);
			}
		}
		assert(false);
		return nullptr;
	}

	template<size_t BufferCapacity>
	template<std::integral T>
	inline T MemoryArena<BufferCapacity>::GetID(const void* aDataPtr) const
	{
		for (size_t i = 0; i < mMemoryBuffers.size(); ++i)
		{
			const std::unique_ptr<MemoryBuffer>& buffer = mMemoryBuffers[i];
			const size_t ptrDiff = GetPointerDiff(aDataPtr, buffer->GetDataPtr());
			if (ptrDiff < BufferCapacity)
			{
				return static_cast<T>(i * BufferCapacity + (static_cast<size_t>(mMemoryBuffers[i]->GetDataPtr()) + ptrDiff));
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
		mCurrentBufferIndex = mMemoryBuffers.size();
		mMemoryBuffers.emplace_back(std::make_unique<MemoryBuffer>());
	}


	template<size_t BufferCapacity>
	inline MemoryBuffer<BufferCapacity>& MemoryArena<BufferCapacity>::GetCurrentBuffer()
	{
		return *mMemoryBuffers.back();
	}

	template<size_t BufferCapacity>
	inline MemoryBuffer<BufferCapacity>& MemoryArena<BufferCapacity>::GetMemoryBuffer(const void* const aDataPtr)
	{
		for (size_t i = 0; i < mMemoryBuffers.size(); ++i)
		{
			const std::unique_ptr<MemoryBuffer>& buffer = mMemoryBuffers[i];
			const size_t ptrDiff = GetPointerDiff(aDataPtr, buffer->GetDataPtr());
			if (ptrDiff < BufferCapacity)
			{
				return *buffer;
			}
		}

		assert(false);

		return GetCurrentBuffer();

	}



}