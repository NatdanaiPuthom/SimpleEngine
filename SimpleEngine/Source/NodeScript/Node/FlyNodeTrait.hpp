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
		Wildcard = 1 << 4,
		TemplateSpec = 1 << 5,
		Template = Wildcard | TemplateSpec,
		CustomEvent = 1 << 6,
		HasFlow = 1 << 7,
		HasImplicitFlow = 1 << 8,
		TakesExecutionContext = 1 << 9,
		TakesInternalExecutionContext = 1 << 10,
		Target = 1 << 11,
		Trait = 1 << 12,
		NonTrivial = Invalid | Getter | Setter | Operator | TemplateSpec | CustomEvent | Target,
		Accessor = Getter | Setter,
		All = ~None
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
		IncrementPrefix = 1 << 15,
		DecrementPrefix = 1 << 16,

		Comparator = Equal | NotEqual | Greater | Less | GreaterEqual | LessEqual,
		Logical = And | Or | Not,
		Math = Add | Subtract | Multiply | Divide | Modulo,
		All = Comparator | Logical | Math | Print | IncrementPrefix | DecrementPrefix
	};
}