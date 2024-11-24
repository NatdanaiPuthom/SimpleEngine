#pragma once
#include "../FlyDefines.hpp"
#include <variant>

namespace FLY_NAMESPACE
{
	template<typename T>
	class PtrValueVariant final
	{
		template<typename T>
		struct ToValueVisitor final
		{
			constexpr T& operator()(T& aValue) const
			{
				return aValue;
			}

			constexpr T& operator()(NonOwningPtr<T>& aValue) const
			{
				return *aValue;
			}

			constexpr const T& operator()(const T& aValue) const
			{
				return aValue;
			}

			constexpr const T& operator()(const NonOwningPtr<T>& aValue) const
			{
				return *aValue;
			}
		};
	public:

		PtrValueVariant() = default;

		PtrValueVariant(const T& aValue)
			: mData(aValue)
		{
		}

		PtrValueVariant(T&& aValue)
			: mData(aValue)
		{
		}

		PtrValueVariant(T* aValue)
			: mData(aValue)
		{
		}

		PtrValueVariant& operator=(const T& aValue)
		{
			mData = aValue;
			return *this;
		}

		PtrValueVariant& operator=(T&& aValue)
		{
			mData = aValue;
			return *this;
		}

		PtrValueVariant& operator=(T* aValue)
		{
			mData = aValue;
			return *this;
		}

		constexpr T& ToValue()
		{
			return std::visit(ToValueVisitor<T>{}, mData);
		}

		constexpr const T& ToValue() const
		{
			return std::visit(ToValueVisitor<T>{}, mData);
		}

		constexpr operator T&()
		{
			return ToValue();
		}

	private:

		std::variant<T, NonOwningPtr<T>> mData;
	};
}