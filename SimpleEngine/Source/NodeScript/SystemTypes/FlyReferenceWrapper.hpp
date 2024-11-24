#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	template<typename T>
	class ReferenceWrapper final
	{
		using Ref = T&;
		using Ptr = T*;
		using PtrRef = T*&;
	public:
		static_assert(std::is_object_v<T> || std::is_function_v<T>,
			"ReferenceWrapper<T> requires T to be an object type or a function type.");

		ReferenceWrapper() = default;

		ReferenceWrapper(Ref aRef)
			: myPtr(&aRef)
		{
		}

		ReferenceWrapper& operator=(T&& aValue)
		{
			Set(aValue);

			return *this;
		}

		constexpr operator Ref() const noexcept
		{
			return *myPtr;
		}

		[[nodiscard]] constexpr const T& Get() const noexcept
		{
			return *myPtr;
		}

		[[nodiscard]] constexpr T& Get() noexcept
		{
			return *myPtr;
		}

		[[nodiscard]] constexpr T*& GetPtr() noexcept
		{
			return myPtr;
		}

		[[nodiscard]] constexpr const T*& GetPtr() const noexcept
		{
			return myPtr;
		}

		constexpr void Set(Ref aValue)
		{
			myPtr = &aValue;
		}

	private:
		Ptr myPtr{};

	public:
		template<typename... Types>
		constexpr auto operator()(Types&&... _Args) const
			noexcept(noexcept(std::invoke(*myPtr, static_cast<Types&&>(_Args)...)))
			-> decltype(std::invoke(*myPtr, static_cast<Types&&>(_Args)...))
		{
			return std::invoke(*myPtr, static_cast<Types&&>(_Args)...);
		}
	};
}