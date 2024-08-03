#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	enum class eNodeTrait : size_t
	{
		None = 0,
		Invalid = 1 << 0,
		Getter = 1 << 1,
		Setter = 1 << 2,
		Operator = 1 << 3,
		Template = 1 << 4,
		CustomEvent = 1 << 5,
		HasFlow = 1 << 6,
		HasImplicitFlow = 1 << 7,
		TakesExecutionContext = 1 << 8,
		TakesInternalExecutionContext = 1 << 9,
		Self = 1 << 10,
		NonTrivial = Invalid | Getter | Setter | Operator | CustomEvent | Self,
		Accessor = Getter | Setter,
		All = Invalid | Getter | Setter | Operator | CustomEvent | HasFlow | TakesExecutionContext | TakesInternalExecutionContext | HasImplicitFlow | Self
	};

	/*enum class eCreationNodeTrait : unsigned int
	{
		HasImplicitFlow,
		TakesExecutionContext
	};*/

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