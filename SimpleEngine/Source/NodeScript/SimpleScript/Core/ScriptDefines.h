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
	using ScriptID = int;
	using NodeID = ScriptID;
	using PinID = ScriptID;
	using LinkID = ScriptID;

	using ScriptID_size_t = size_t;
	using NodeTypeID = ScriptID_size_t;
	using CustomEventID = ScriptID_size_t;
	using FunctionID = ScriptID_size_t;
	using PinTypeID = ScriptID_size_t;
	using VarID = ScriptID_size_t;
	using MemoryPoolID = size_t;
	using DataTypeID = ScriptID_size_t;
	using ObjectTypeID = ScriptID_size_t;

	template<std::integral IDType>
	inline constexpr IDType InvalidID()
	{
		return std::numeric_limits<IDType>::max();
	}

	template<typename Output, typename... Inputs>
	using FuncPtr = Output(*)(Inputs...);

	template<typename ClassType, typename OutputType, typename... InputTypes>
	using FuncPtrMember = OutputType(ClassType::*)(InputTypes...);

	template<typename ClassType, typename OutputType, typename... InputTypes>
	using FuncPtrMember_Const = OutputType(ClassType::*)(InputTypes...) const;

	class Node;
	class ScriptInternalModifier;
	struct InternalExecutionContext;
	struct NodeExecutionData;

	using CreateNodeSignature = Node(*)(const NodeID, const NodeTypeID, ScriptInternalModifier&);
	using ExecuteNodeSignature = void(*)(const NodeExecutionData&, InternalExecutionContext&);

	// Struct for color - values between 0 and 1
	struct Color
	{
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