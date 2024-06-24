#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	enum class eNodeTrait : size_t
	{
		None = 0,
		HasImplicitFlow = 1 << 0,
		TakesExecutionContext = 1 << 1,
		Invalid = 1 << 2,
		Getter = 1 << 3,
		Setter = 1 << 4,
		Operator = 1 << 5,
		CustomEvent = 1 << 6,
		HasFlow = 1 << 7,
		NonTrivial = Invalid | Getter | Setter | Operator | CustomEvent,
		Accessor = Getter | Setter,
		TakesInternalExecutionContext = 1 << 8
	};

	enum class eCreationNodeTrait : unsigned int
	{
		HasImplicitFlow,
		TakesExecutionContext
	};

	enum class eNodeEventType : unsigned int
	{
		None,
		BeginPlay,
		Tick,
		EndPlay,
		Custom,
	};

	enum class eNodeOperatorTrait : unsigned int
	{
		None = 0,
		Equal = 1 << 0,
		NotEqual = 1 << 1,
		Greater = 1 << 2,
		Less = 1 << 3,
		GreaterEqual = 1 << 4,
		LessEqual = 1 << 5,
		And = 1 << 6,
		Or = 1 << 7,
		Not = 1 << 8,
		Print = 1 << 9,
		Add = 1 << 10,
		Subtract = 1 << 11,
		Multiply = 1 << 12,
		Divide = 1 << 13,
		Modulo = 1 << 14,
		Functor = 1 << 15,

		Comparator = Equal | NotEqual | Greater | Less | GreaterEqual | LessEqual,
		Logical = And | Or | Not,
		Math = Add | Subtract | Multiply | Divide | Modulo,
		All = Comparator | Logical | Math | Print | Functor
	};
}