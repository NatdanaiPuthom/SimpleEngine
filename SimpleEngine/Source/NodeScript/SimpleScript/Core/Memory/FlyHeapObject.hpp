#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <memory>

namespace FLY_NAMESPACE
{
	template<typename T, bool Nullable>
	concept DC_HO = Nullable || (std::default_initializable<T> && !Nullable);

	template<typename T>
	class HeapObject final
	{
	public:

		HeapObject() = default;
		~HeapObject() = default;
		explicit HeapObject(T* aPtr);

		HeapObject(const HeapObject& aOther);
		HeapObject(HeapObject&&) noexcept = default;

		HeapObject& operator=(const HeapObject& aOther);
		HeapObject& operator=(HeapObject&&) noexcept = default;

		[[nodiscard]] T* Get() noexcept;
		[[nodiscard]] const T* Get() const noexcept;

		[[nodiscard]] T* operator->() noexcept;
		[[nodiscard]] const T* operator->() const noexcept;
		[[nodiscard]] T& operator*() noexcept;
		[[nodiscard]] const T& operator*() const noexcept;

		void Reset(T* aPointer = nullptr);
		[[nodiscard]] T* Release();

		explicit operator bool() const noexcept;

	private:

		std::unique_ptr<T> mUniquePtr;
	};

	template<typename T>
	inline HeapObject<T>::HeapObject(T* aPtr)
		: mUniquePtr(aPtr)
	{
	}

	template<typename T>
	inline HeapObject<T>::HeapObject(const HeapObject& aOther)
		: mUniquePtr(aOther ? std::make_unique<T>(*aOther) : std::unique_ptr<T>())
	{
	}

	template<typename T>
	inline HeapObject<T>& HeapObject<T>::operator=(const HeapObject& aOther)
	{
		mUniquePtr = aOther ? std::make_unique<T>(*aOther) : std::unique_ptr<T>();

		return *this;
	}

	template<typename T>
	inline T* HeapObject<T>::Get() noexcept
	{
		return mUniquePtr.get();
	}

	template<typename T>
	inline const T* HeapObject<T>::Get() const noexcept
	{
		return mUniquePtr.get();
	}

	template<typename T>
	inline T* HeapObject<T>::operator->() noexcept
	{
		return Get();
	}

	template<typename T>
	inline const T* HeapObject<T>::operator->() const noexcept
	{
		return Get();
	}

	template<typename T>
	inline T& HeapObject<T>::operator*() noexcept
	{
		return *Get();
	}
	template<typename T>
	inline const T& HeapObject<T>::operator*() const noexcept
	{
		return *Get();
	}

	template<typename T>
	inline void HeapObject<T>::Reset(T* aPointer)
	{
		mUniquePtr.reset(aPointer);
	}
	template<typename T>
	inline T* HeapObject<T>::Release()
	{
		return mUniquePtr.release();
	}

	template<typename T>
	inline HeapObject<T>::operator bool() const noexcept
	{
		return mUniquePtr.operator bool();
	}

	template<typename T, class... Args> requires Fly::HasArgsConstructor<T, Args...>
	[[nodiscard]] inline HeapObject<T> MakeHeapObject(Args&&... aArgs)
	{
		return HeapObject<T>(new T(std::forward<Args>(aArgs)...));
	}

}