#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/MetaScript.h"
#include "../Utilities/ScriptUtilities.h"
#include "../DataType/DataTypeManager.h"
#include "../SystemTypes/ScriptWildcard.h"
#include "../Node/NodeTrait.h"
#include "../Node/NodeTypeRegistry.h"

namespace SCR
{

	struct OperatorNodes
	{
		template<HasOperator_EqualTo T>
		static constexpr inline bool EqualTo(T aValue1, T aValue2)
		{
			return aValue1 == aValue2;
		}

		template<HasOperator_NotEqualTo T>
		static constexpr inline bool NotEqualTo(T aValue1, T aValue2)
		{
			return aValue1 != aValue2;
		}

		template<HasOperator_Greater T>
		static constexpr inline bool Greater(T aValue1, T aValue2)
		{
			return aValue1 > aValue2;
		}

		template<HasOperator_Less T>
		static constexpr inline bool Less(T aValue1, T aValue2)
		{
			return aValue1 < aValue2;
		}

		template<HasOperator_GreaterEqual T>
		static constexpr  inline bool GreaterEqual(T aValue1, T aValue2)
		{
			return aValue1 >= aValue2;
		}

		template<HasOperator_LessEqual T>
		static constexpr inline bool LessEqual(T aValue1, T aValue2)
		{
			return aValue1 <= aValue2;
		}

		template<HasOperator_And T>
		static constexpr inline T And(T aValue1, T aValue2)
		{
			return aValue1 && aValue2;
		}

		template<HasOperator_Or T>
		static constexpr inline T Or(T aValue1, T aValue2)
		{
			return aValue1 || aValue2;
		}

		template<HasOperator_Not T>
		static constexpr inline T Not(T aValue)
		{
			return !aValue;
		}

		template<HasOperator_Print T>
		static constexpr inline void Print(T aValue)
		{
			std::cout << aValue << std::endl;
		}

		template<HasOperator_Add T>
		static constexpr inline T Add(T aValue1, T aValue2)
		{
			return aValue1 + aValue2;
		}

		template<HasOperator_Subtract T>
		static constexpr inline T Substract(T aValue1, T aValue2)
		{
			return aValue1 - aValue2;
		}

		template<HasOperator_Multiply T>
		static constexpr inline T Multiply(T aValue1, T aValue2)
		{
			return aValue1 * aValue2;
		}

		template<HasOperator_Divide T>
		static constexpr inline T Divide(T aValue1, T aValue2)
		{
			return aValue1 / aValue2;
		}

		template<HasOperator_Modulo T>
		static constexpr inline T Modulo(T aValue1, T aValue2)
		{
			return aValue1 % aValue2;
		}

		template<HasOperator_IncrementPrefix T>
		static constexpr inline T& IncrementPrefix(T& aValue)
		{
			return ++aValue;
		}

		template<HasOperator_IncrementPostfix T>
		static constexpr inline T IncrementPostfix(T aValue)
		{
			aValue++;
			return aValue;
		}

		template<HasOperator_DecrementPrefix T>
		static constexpr inline T& DecrementPrefix(T& aValue)
		{
			return --aValue;
		}

		template<HasOperator_DecrementPostfix T>
		static constexpr inline T DecrementPostfix(T aValue)
		{
			aValue--;
			return aValue;
		}

		template<HasOperator_Functor T>
		static constexpr inline void Functor(T aValue)
		{
			aValue();
		}
	};


	template<typename T>
	inline bool AssertOperator()
	{
		return false;
	}

	template<typename CastTo, IsStaticCastable<CastTo> CastFrom>
	inline CastTo StaticCastNodeTemplate(CastFrom aValue)
	{
		return static_cast<CastTo>(aValue);
	}

	template<CleanType T, eNodeOperatorTrait OperatorTrait>
	constexpr inline bool HasOperator()
	{
		switch (OperatorTrait)
		{
		case eNodeOperatorTrait::None:
			break;
		case eNodeOperatorTrait::Equal:
			return HasOperator_EqualTo<T>;
			break;
		case eNodeOperatorTrait::NotEqual:
			return HasOperator_NotEqualTo<T>;
			break;
		case eNodeOperatorTrait::Greater:
			return HasOperator_Greater<T>;
			break;
		case eNodeOperatorTrait::Less:
			return HasOperator_Less<T>;
			break;
		case eNodeOperatorTrait::GreaterEqual:
			return HasOperator_GreaterEqual<T>;
			break;
		case eNodeOperatorTrait::LessEqual:
			return HasOperator_LessEqual<T>;
			break;
		case eNodeOperatorTrait::And:
			return HasOperator_And<T>;
			break;
		case eNodeOperatorTrait::Or:
			return HasOperator_Or<T>;
			break;
		case eNodeOperatorTrait::Not:
			return HasOperator_Not<T>;
			break;
		case eNodeOperatorTrait::Print:
			return HasOperator_Print<T>;
			break;
		case eNodeOperatorTrait::Add:
			return HasOperator_Add<T>;
			break;
		case eNodeOperatorTrait::Subtract:
			return HasOperator_Subtract<T>;
			break;
		case eNodeOperatorTrait::Multiply:
			return HasOperator_Multiply<T>;
			break;
		case eNodeOperatorTrait::Divide:
			return HasOperator_Divide<T>;
			break;
		case eNodeOperatorTrait::Modulo:
			return HasOperator_Modulo<T>;
			break;
			/*	case eNodeOperatorTrait::IncrementPrefix:
					return HasOperator_IncrementPrefix<T>;
					break;
				case eNodeOperatorTrait::IncrementPostfix:
					return HasOperator_IncrementPostfix<T>;
					break;
				case eNodeOperatorTrait::DecrementPrefix:
					return HasOperator_DecrementPrefix<T>;
					break;
				case eNodeOperatorTrait::DecrementPostfix:
					return HasOperator_DecrementPostfix<T>;
					break;*/
		case eNodeOperatorTrait::Functor:
			return HasOperator_Functor<T>;
			break;
		default:
			break;
		}
		return false;
	}

	template<CleanType T, eNodeOperatorTrait OperatorTrait>
	constexpr inline auto GetFunctionByOperator()
	{
		if constexpr (OperatorTrait == eNodeOperatorTrait::Equal)
		{
			return OperatorNodes::EqualTo<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::NotEqual)
		{
			return OperatorNodes::NotEqualTo<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Greater)
		{
			return OperatorNodes::Greater<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Less)
		{
			return OperatorNodes::Less<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::GreaterEqual)
		{
			return OperatorNodes::GreaterEqual<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::LessEqual)
		{
			return OperatorNodes::LessEqual<T>;
		}

		else if constexpr (OperatorTrait == eNodeOperatorTrait::And)
		{
			return OperatorNodes::And<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Or)
		{
			return OperatorNodes::Or<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Not)
		{
			return OperatorNodes::Not<T>;
		}


		else if constexpr (OperatorTrait == eNodeOperatorTrait::Print)
		{
			return OperatorNodes::Print<T>;
		}


		else if constexpr (OperatorTrait == eNodeOperatorTrait::Add)
		{
			return OperatorNodes::Add<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Subtract)
		{
			return OperatorNodes::Substract<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Multiply)
		{
			return OperatorNodes::Multiply<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Divide)
		{
			return OperatorNodes::Divide<T>;
		}
		else if constexpr (OperatorTrait == eNodeOperatorTrait::Modulo)
		{
			return OperatorNodes::Modulo<T>;
		}

		/*	else if constexpr (OperatorTrait == eNodeOperatorTrait::IncrementPrefix)
			{
				return OperatorNodes::IncrementPrefix<T>;
			}
			else if constexpr (OperatorTrait == eNodeOperatorTrait::IncrementPostfix)
			{
				return OperatorNodes::IncrementPostfix<T>;
			}
			else if constexpr (OperatorTrait == eNodeOperatorTrait::DecrementPrefix)
			{
				return OperatorNodes::DecrementPrefix<T>;
			}
			else if constexpr (OperatorTrait == eNodeOperatorTrait::DecrementPostfix)
			{
				return OperatorNodes::DecrementPostfix<T>;
			}*/

		else if constexpr (OperatorTrait == eNodeOperatorTrait::Functor)
		{
			return OperatorNodes::Functor<T>;
		}

		else
		{
			return AssertOperator<T>;
		}
	}


	struct NodeTypeDesc;

	template<eNodeTrait, typename OutputType, typename... InputTypes>
	constexpr void RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...>, const std::string&, NodeTypeDesc);


	template<CleanType T, eNodeOperatorTrait OperatorTrait, eNodeTrait ExtraTraits = eNodeTrait::None>
	constexpr inline void RegisterOperatorNode(std::string aDefaultNodeName)
	{

		const size_t dataTypeID = typeid(T).hash_code();

		if constexpr (!IsSameType<T, Wildcard>)
		{
			const std::string& customTypeName = Global::GetDataTypeManager().GetName(dataTypeID);
			if (!customTypeName.empty())
			{
				aDefaultNodeName += " (" + customTypeName + ")";
			}
			else
			{
				const std::string typeName = typeid(T).name();
				aDefaultNodeName += " (" + typeName + ")";
			}
		}

		constexpr eNodeTrait Traits = (IsSameType<T, Wildcard> ? eNodeTrait::None : eNodeTrait::Operator) | ExtraTraits;

		auto func = GetFunctionByOperator<T, OperatorTrait>();

		NodeTypeID nodeTypeID = RegisterSystemNodeType<Traits, OperatorTrait>(func, aDefaultNodeName);

		NodeTypeManager::GetInstance().SetOperatorNodeTypeID(dataTypeID, OperatorTrait, nodeTypeID);
	}

	template<CleanType T, eNodeOperatorTrait OperatorTrait, eNodeOperatorTrait RegisteredTraits, eNodeTrait ExtraTraits = eNodeTrait::None>
	constexpr inline void TryRegisterOperatorNode(std::string aDefaultNodeName)
	{
		if constexpr (HasFlag(OperatorTrait, RegisteredTraits))
		{
			if constexpr (HasOperator<T, OperatorTrait>())
			{
				RegisterOperatorNode<T, OperatorTrait, ExtraTraits>(aDefaultNodeName);
			}
		}

	}

	template<CleanType T, eNodeOperatorTrait RegisteredTraits>
	inline void RegisterOperatorNodeTypes()
	{
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Equal, RegisteredTraits>("Operators/Compare/Equal To");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::NotEqual, RegisteredTraits>("Operators/Compare/Not Equal To");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Greater, RegisteredTraits>("Operators/Compare/Greater Than");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Less, RegisteredTraits>("Operators/Compare/Less Than");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::GreaterEqual, RegisteredTraits>("Operators/Compare/Greater Or Equal To");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::LessEqual, RegisteredTraits>("Operators/Compare/Less Or Equal To");

		TryRegisterOperatorNode<T, eNodeOperatorTrait::And, RegisteredTraits>("Operators/Logical/And");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Or, RegisteredTraits>("Operators/Logical/Or");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Not, RegisteredTraits>("Operators/Logical/Not");

		TryRegisterOperatorNode<T, eNodeOperatorTrait::Print, RegisteredTraits, eNodeTrait::HasImplicitFlow>("Operators/Print");

		TryRegisterOperatorNode<T, eNodeOperatorTrait::Add, RegisteredTraits>("Operators/Math/Add");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Subtract, RegisteredTraits>("Operators/Math/Subtract");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Multiply, RegisteredTraits>("Operators/Math/Multiply");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Divide, RegisteredTraits>("Operators/Math/Divide");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::Modulo, RegisteredTraits>("Operators/Math/Modulo");

		TryRegisterOperatorNode<T, eNodeOperatorTrait::Functor, RegisteredTraits, eNodeTrait::HasImplicitFlow>("Operators/Functor");

	}
}