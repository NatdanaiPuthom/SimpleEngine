#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <memory>

namespace FLY_NAMESPACE
{

	template<typename T, bool Nullable>
	class HeapObject;


	template<typename T>
	class HeapObject<T, true> final
	{
	public:

		HeapObject() = default;
		explicit HeapObject(T* aPtr)
			: mUniquePtr(std::unique_ptr<T>(aPtr))
		{
		}


		template<typename... Args> requires HasArgsConstructor<T, Args...>
		HeapObject(Args&&... aArgs)
			: mUniquePtr(std::make_unique<T>(std::forward<Args>(aArgs)...))
		{
		}

		HeapObject(const HeapObject& aOther)
			: mUniquePtr(aOther ? std::make_unique<T>(*aOther) : std::unique_ptr<T>())
		{

		}

		HeapObject(HeapObject&&) noexcept = default;

		template<typename T>
		HeapObject& operator=(const HeapObject& aOther)
		{
			mUniquePtr = aOther ? std::unique_ptr<T>(*aOther) : std::unique_ptr<T>();

			return *this;
		}

		HeapObject& operator=(HeapObject&&) noexcept = default;

		[[nodiscard]] T* Get()
		{
			return mUniquePtr.get();
		}

		[[nodiscard]] const T* Get() const
		{
			return mUniquePtr.get();
		}

		[[nodiscard]] T* operator->()
		{
			return Get();
		}

		[[nodiscard]] const T* operator->() const
		{
			return Get();
		}

		[[nodiscard]] T& operator*()
		{
			return *mUniquePtr;
		}

		[[nodiscard]] const T& operator*() const
		{
			return *mUniquePtr;
		}

		void Reset(T* aPointer = nullptr)
		{
			mUniquePtr.reset(aPointer);
		}

		[[nodiscard]] T* Release()
		{
			return mUniquePtr.release();
		}

		explicit operator bool() const
		{
			return mUniquePtr.operator bool();
		}

	private:

		std::unique_ptr<T> mUniquePtr;

	};

	template<typename T>
	class HeapObject<T, false> final
	{
	public:

		template<typename... Args> requires HasArgsConstructor<T, Args...>
		HeapObject(Args&&... aArgs)
			: mUniquePtr(std::make_unique<T>(std::forward<Args>(aArgs)...))
		{
		}

		HeapObject(const HeapObject& aOther)
			: mUniquePtr(std::make_unique<T>(*aOther))
		{

		}


		HeapObject(HeapObject&&) noexcept = default;

		HeapObject& operator=(const HeapObject& aOther)
		{
			mUniquePtr = std::make_unique<T>(*aOther);

			return *this;
		}

		HeapObject& operator=(HeapObject&&) noexcept = default;

		T& Get()
		{
			return *mUniquePtr.get();
		}

		const T& Get() const
		{
			return *mUniquePtr.get();
		}

		T* operator->()
		{
			return mUniquePtr.get();
		}

		const T* operator->() const
		{
			return mUniquePtr.get();
		}

		T& operator*()
		{
			return *mUniquePtr;
		}

		const T& operator*() const
		{
			return *mUniquePtr;
		}

	private:

		std::unique_ptr<T> mUniquePtr;

	};

}