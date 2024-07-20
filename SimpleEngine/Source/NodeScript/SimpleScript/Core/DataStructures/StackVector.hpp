#pragma once
#include "ScriptDefines.h"
#include <cassert>

namespace FLY_NAMESPACE
{

	template<class T, size_t _Capacity>
	class StackVector;

	template<typename T>
	class StackVector<T, 0>
	{
	public:

		
	};

	template<class T, size_t _Capacity>
	class StackVector
	{
		class Iterator
		{
			using Ptr = T*;
			using Reference = T&;

		public:

			Iterator(Ptr aPtr)
				: myPtr(aPtr)
			{
			}

			Iterator operator++()
			{
				++myPtr;
				return *this;
			}

			Iterator operator++(T)
			{
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			Ptr operator->()
			{
				return myPtr;
			}

			Reference operator*() const
			{
				return *myPtr;
			}

			friend bool operator==(const Iterator& anIterator1, const Iterator& anIterator2)
			{
				return anIterator1.myPtr == anIterator2.myPtr;
			}

			friend bool operator!=(const Iterator& anIterator1, const Iterator& anIterator2)
			{
				return anIterator1.myPtr != anIterator2.myPtr;
			}

		private:

			Ptr myPtr;

		};

	public:

		StackVector()
			: myValues()
			, mySize(0)
		{
		}
		~StackVector()
		{

		}

		T& EmplaceBack()
		{
			assert(mySize < _Capacity);
			const size_t currentSize = mySize;
			++mySize;
			return myValues[currentSize];
		}

		void PushBack(const T& aValue)
		{
			assert(mySize < _Capacity);
			myValues[mySize] = aValue;
			++mySize;
		}
		T& Back()
		{
			return myValues[mySize - 1];
		}

		T& operator[](const size_t anIndex) noexcept
		{
			assert(anIndex < mySize);
			return myValues[anIndex];
		}

		const T& operator[](const size_t anIndex) const noexcept
		{
			assert(anIndex < mySize);
			return myValues[anIndex];
		}

		void operator++()
		{
			assert(mySize + 1 < _Capacity);
			++mySize;
		}
		size_t Size() const noexcept
		{
			return mySize;
		}

		constexpr size_t Capacity() const noexcept
		{
			return _Capacity;
		}

		T* Data() noexcept
		{
			return &myValues[0];
		}

		constexpr Iterator begin() const noexcept
		{
			return Iterator(&myValues[0]);
		}

		constexpr Iterator end() const noexcept
		{
			return Iterator(&myValues[mySize]);
		}

	private:

		T myValues[_Capacity];
		size_t mySize;
	};
}