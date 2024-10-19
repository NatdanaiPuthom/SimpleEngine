#pragma once
#include "../FlyDefines.hpp"
#include <string>
#include <variant>

namespace FLY_NAMESPACE
{
	struct Variable final
	{
		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		OwningPtr<void> mDefaultValueDataPtr = nullptr;
		std::string mName = "Var";
		size_t mByteOffset = 0;
		bool mIsDestroyed = false;
	};

	template<typename T>
	class DynamicVariable final
	{
		template<typename T>
		struct ToValueVisitor final
		{
			T& operator()(T& aValue) const
			{
				return aValue;
			}

			T& operator()(Fly::NonOwningPtr<T>& aValue) const
			{
				return *aValue;
			}

			const T& operator()(const T& aValue) const
			{
				return aValue;
			}

			const T& operator()(const Fly::NonOwningPtr<T>& aValue) const
			{
				return *aValue;
			}
		};
	public:

		DynamicVariable() = default;

		DynamicVariable(const T& aValue)
			: mData(aValue)
		{
		}

		DynamicVariable(T&& aValue)
			: mData(aValue)
		{
		}

		DynamicVariable(T* aValue)
			: mData(aValue)
		{
		}

		DynamicVariable& operator=(const T& aValue)
		{
			mData = aValue;
			return *this;
		}

		DynamicVariable& operator=(T&& aValue)
		{
			mData = aValue;
			return *this;
		}

		DynamicVariable& operator=(T* aValue)
		{
			mData = aValue;
			return *this;
		}

		T& ToValue()
		{
			return std::visit(ToValueVisitor<int>{}, mData);
		}

		const T& ToValue() const
		{
			return std::visit(ToValueVisitor<int>{}, mData);
		}

	private:

		std::variant<T, Fly::NonOwningPtr<T>> mData;
	};
}