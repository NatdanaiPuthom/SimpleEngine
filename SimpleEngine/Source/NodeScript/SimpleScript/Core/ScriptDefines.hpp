#pragma once
#define SCR SCRIPT
#if defined _DEBUG
#define FLY_DEBUG
#endif
#include <limits>
#include <type_traits>
#include <algorithm>

#undef max

namespace SCR
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

	constexpr size_t NodeBufferCapacity = 1024;
	constexpr DataTypeID GlobalDataTypeID = 0;

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

	// Struct for color - values between 0 and 1
	struct Color
	{
		Color(float aR, float aG, float aB, float aA = 1.f)
			: r(aR)
			, g(aG)
			, b(aB)
			, a(aA)
		{

		}
		float r = 0, g = 0, b = 0, a = 1;

		void Clamp()
		{
			r = std::clamp(r, 0.f, 1.f);
			g = std::clamp(g, 0.f, 1.f);
			b = std::clamp(b, 0.f, 1.f);
			a = std::clamp(a, 0.f, 1.f);
		}
	};


	inline Color operator+(const Color& aColor1, const Color& aColor2)
	{
		Color c = { aColor1.r + aColor2.r, aColor1.g + aColor2.g, aColor1.b + aColor2.b, aColor1.a + aColor2.a };
		c.Clamp();
		return c;
	}

	inline Color operator-(const Color& aColor1, const Color& aColor2)
	{
		Color c = { aColor1.r - aColor2.r, aColor1.g - aColor2.g, aColor1.b - aColor2.b, aColor1.a - aColor2.a };
		c.Clamp();
		return c;
	}
}