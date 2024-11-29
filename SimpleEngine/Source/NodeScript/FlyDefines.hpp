#pragma once
#define FLY_NAMESPACE Fly
#if defined _DEBUG
#define FLY_DEBUG
#endif
#include <limits>
#include <type_traits>
#include <algorithm>
#include <string_view>
#include <string>
#include "SystemTypes/FlyNone.hpp"

#undef max

namespace FLY_NAMESPACE
{
	using NodeID = int;
	using PinID = int;
	using LinkID = int;

	using NodeTypeID = size_t;
	using CustomEventID = size_t;
	using FunctionID = size_t;
	using PinTypeID = size_t;
	using VarID = size_t;
	using MemoryPoolID = size_t;
	using DataTypeID = size_t;
	using ObjectTypeID = size_t;
	using EventID = size_t;

	constexpr const char* TypeIdentifierStr = "#T";

	template<std::integral IDType>
	inline constexpr IDType InvalidID()
	{
		return std::numeric_limits<IDType>::max();
	}

	template<typename T>
	inline constexpr DataTypeID GetDataTypeID()
	{
		return typeid(T).hash_code();
	}

	template<typename Output, typename... Inputs>
	using FuncPtr = Output(*)(Inputs...);

	template<typename ClassType, typename OutputType, typename... InputTypes>
	using FuncPtrMember = OutputType(ClassType::*)(InputTypes...);

	template<typename ClassType, typename OutputType, typename... InputTypes>
	using FuncPtrMember_Const = OutputType(ClassType::*)(InputTypes...) const;

	class Node;
	class NodeGraph;
	struct InternalExecutionContext;
	struct NodeExecutionData;
	class ExecutionQueue;

	using CreateNodeSignature = Node(*)(const NodeID, const NodeTypeID, NodeGraph&);
	using ExecuteNodeSignature = void(*)(const NodeExecutionData&, InternalExecutionContext&);

	struct SetPinValueData;
	struct SetPinValueFromPinData;
	using SetPinValueInterface = void(*)(const SetPinValueData&, const InternalExecutionContext&);
	using SetPinValueFromPinInterface = void(*)(const SetPinValueFromPinData&, const InternalExecutionContext&);

	template<typename T>
	class OwningPtr final
	{
	public:
		OwningPtr(T* const aPtr)
			: mPtr(aPtr)
		{
		}

		T* Get() noexcept
		{
			return mPtr;
		}

		const T* Get() const noexcept
		{
			return mPtr;
		}

		operator T* () noexcept
		{
			return Get();
		}

		operator const T* () const noexcept
		{
			return Get();
		}

	private:
		T* mPtr;
	};

	template<typename T>
	class NonOwningPtr final
	{
	public:

		constexpr NonOwningPtr(T* aPtr)
			: mPtr(aPtr)
		{
		}

		constexpr T* Get() const noexcept
		{
			return mPtr;
		}

		constexpr explicit operator T* () const noexcept
		{
			return Get();
		}

		constexpr T& operator*() const
		{
			return *mPtr;
		}

	private:

		T* mPtr;
	};

	// Struct for color - values between 0 and 1
	struct Color final
	{

		constexpr Color() = default;
		constexpr Color(float aR, float aG, float aB, float aA = 1.f)
			: r(aR)
			, g(aG)
			, b(aB)
			, a(aA)
		{

		}
		float r = 0, g = 0, b = 0, a = 1;

		constexpr void Clamp()
		{
			r = std::clamp(r, 0.f, 1.f);
			g = std::clamp(g, 0.f, 1.f);
			b = std::clamp(b, 0.f, 1.f);
			a = std::clamp(a, 0.f, 1.f);
		}

	};

	constexpr inline Color operator+(const Color& aColor1, const Color& aColor2)
	{
		Color c = { aColor1.r + aColor2.r, aColor1.g + aColor2.g, aColor1.b + aColor2.b, aColor1.a + aColor2.a };
		c.Clamp();
		return c;
	}

	constexpr inline Color operator-(const Color& aColor1, const Color& aColor2)
	{
		Color c = { aColor1.r - aColor2.r, aColor1.g - aColor2.g, aColor1.b - aColor2.b, aColor1.a - aColor2.a };
		c.Clamp();
		return c;
	}

	namespace Colors
	{
		constexpr Color Black = Color();
		constexpr Color White = Color(1.f, 1.f, 1.f);
		constexpr Color Red = Color(1.f, 0.f, 0.f);
		constexpr Color Blue = Color(0.f, 0.f, 1.f);
		constexpr Color Green = Color(0.f, 1.f, 0.f);
		constexpr Color Yellow = Color(1.f, 1.f, 0.f);
		constexpr Color Orange = Color(1.f, 0.65f, 0.f);
		constexpr Color Pink = Color(1.f, 0.57f, 0.69f);
		constexpr Color Purple = Color(0.6f, 0.f, 0.1f);
		constexpr Color Gray = Color(0.5f, 0.5f, 0.5f);
	}

	struct EditAndViewResult final
	{
		bool mIsItemActive = false;
	};
}