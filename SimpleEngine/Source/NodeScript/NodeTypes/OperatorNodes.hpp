#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../SystemTypes/FlyWildcard.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	struct OperatorNodes
	{
		template<HasOperator_EqualTo T>
		static constexpr inline bool EqualTo(const T& aValue1, const T& aValue2)
		{
			return aValue1 == aValue2;
		}

		template<HasOperator_NotEqualTo T>
		static constexpr inline bool NotEqualTo(const T& aValue1, const T& aValue2)
		{
			return aValue1 != aValue2;
		}

		template<HasOperator_Greater T>
		static constexpr inline bool Greater(const T& aValue1, const T& aValue2)
		{
			return aValue1 > aValue2;
		}

		template<HasOperator_Less T>
		static constexpr inline bool Less(const T& aValue1, const T& aValue2)
		{
			return aValue1 < aValue2;
		}

		template<HasOperator_GreaterEqual T>
		static constexpr  inline bool GreaterEqual(const T& aValue1, const T& aValue2)
		{
			return aValue1 >= aValue2;
		}

		template<HasOperator_LessEqual T>
		static constexpr inline bool LessEqual(const T& aValue1, const T& aValue2)
		{
			return aValue1 <= aValue2;
		}

		template<HasOperator_And T>
		static constexpr inline T And(const T& aValue1, const T& aValue2)
		{
			return aValue1 && aValue2;
		}

		template<HasOperator_Or T>
		static constexpr inline T Or(const T& aValue1, const T& aValue2)
		{
			return aValue1 || aValue2;
		}

		template<HasOperator_Not T>
		static constexpr inline T Not(const T& aValue)
		{
			return !aValue;
		}

		template<HasOperator_Print T>
		static constexpr inline void Print(const T& aValue)
		{
			std::cout << aValue << '\n';
		}

		template<HasOperator_Add T>
		static constexpr inline T Add(const T& aValue1, const T& aValue2)
		{
			return aValue1 + aValue2;
		}

		template<HasOperator_Subtract T>
		static constexpr inline T Substract(const T& aValue1, const T& aValue2)
		{
			return aValue1 - aValue2;
		}

		template<HasOperator_Multiply T>
		static constexpr inline T Multiply(const T& aValue1, const T& aValue2)
		{
			return aValue1 * aValue2;
		}

		template<HasOperator_Divide T>
		static constexpr inline T Divide(const T& aValue1, const T& aValue2)
		{
			return aValue1 / aValue2;
		}

		template<HasOperator_Modulo T>
		static constexpr inline T Modulo(const T& aValue1, const T& aValue2)
		{
			return aValue1 % aValue2;
		}

		template<HasOperator_IncrementPrefix T>
		static constexpr inline void IncrementPrefix(T* aValue)
		{
			if (!aValue)
			{
				return;
			}
			++(*aValue);
		}

		template<HasOperator_IncrementPostfix T>
		static constexpr inline void IncrementPostfix(T* aValue)
		{
			if (!aValue)
			{
				return;
			}
			(*aValue)++;
		}

		template<HasOperator_DecrementPrefix T>
		static constexpr inline void DecrementPrefix(T* aValue)
		{
			if (!aValue)
			{
				return;
			}
			--(*aValue);
		}

		template<HasOperator_DecrementPostfix T>
		static constexpr inline void DecrementPostfix(T* aValue)
		{
			if (!aValue)
			{
				return;
			}
			(*aValue)--;
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

	template<Decayed T, eNodeOperatorTrait OperatorTrait>
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
		case eNodeOperatorTrait::IncrementPrefix:
			return HasOperator_IncrementPrefix<T>;
			break;
		default:
			break;
		}
		return false;
	}

	template<Decayed T, eNodeOperatorTrait OperatorTrait>
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

		else if constexpr (OperatorTrait == eNodeOperatorTrait::IncrementPrefix)
		{
			return OperatorNodes::IncrementPrefix<T>;
		}

		else if constexpr (OperatorTrait == eNodeOperatorTrait::DecrementPrefix)
		{
			return OperatorNodes::DecrementPrefix<T>();
		}

		else
		{
			return AssertOperator<T>;
		}
	}


	/*struct NodeTypeDesc;

	template<eNodeTrait, typename OutputType, typename... InputTypes>
	constexpr void RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...>, NodeCreationData&& = NodeCreationData());*/


	template<Decayed T, eNodeOperatorTrait OperatorTrait, eNodeTrait ExtraTraits = eNodeTrait::None>
	constexpr inline void RegisterOperatorNode(std::string aDefaultNodeName)
	{

		const DataTypeID dataTypeID = GetDataTypeID<T>();

		if constexpr (!std::same_as<T, Wildcard>)
		{
			const std::string& customTypeName = Internal::GetDataTypeManager().Find(dataTypeID)->Name();
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

		constexpr eNodeTrait Traits = (std::same_as<T, Wildcard> ? eNodeTrait::None : eNodeTrait::Operator) | ExtraTraits;

		auto operatorFunc = GetFunctionByOperator<T, OperatorTrait>();

		const NodeTypeID nodeTypeID = RegisterSystemNodeType<Traits>(operatorFunc, NodeCreationData{ .mName = aDefaultNodeName, .mOperatorTrait = OperatorTrait });

		Internal::GetNodeTypeManager().SetOperatorNodeTypeID(dataTypeID, OperatorTrait, nodeTypeID);
	}

	template<Decayed T, eNodeOperatorTrait OperatorTrait, eNodeOperatorTrait RegisteredTraits, eNodeTrait ExtraTraits = eNodeTrait::None>
	constexpr inline void TryRegisterOperatorNode(const std::string& aDefaultNodeName)
	{
		if constexpr (HasFlag(OperatorTrait, RegisteredTraits))
		{
			if constexpr (HasOperator<T, OperatorTrait>())
			{
				RegisterOperatorNode<T, OperatorTrait, ExtraTraits>(aDefaultNodeName);
			}
		}

	}

	template<Decayed T, eNodeOperatorTrait RegisteredTraits>
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

		TryRegisterOperatorNode<T, eNodeOperatorTrait::IncrementPrefix, RegisteredTraits, eNodeTrait::HasImplicitFlow>("Operators/Math/Increment");
		TryRegisterOperatorNode<T, eNodeOperatorTrait::DecrementPrefix, RegisteredTraits, eNodeTrait::HasImplicitFlow>("Operators/Math/Decrement");

	}
}