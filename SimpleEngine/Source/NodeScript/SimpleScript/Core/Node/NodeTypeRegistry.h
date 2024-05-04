#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/ScriptUtilities.h"
#include "NodeCreator.h"
#include "NodeTypeManager.h"
#include "../Utilities/MetaScript.h"
#include "../Pin/PinTypeManager.h"
#include "../DataType/DataTypeManager.h"

namespace SCR
{
	inline NodeTypeID RegisterInternal(NodeRecipe&& aNodeRecipe, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
	{
		const std::string defaultPinNames = aDescription.showDataTypePinNames ? "#T" : "";
		aDescription.inputPinNames.resize(aNodeRecipe.inputPinTypeIDs.size(), defaultPinNames);
		aDescription.outputPinNames.resize(aNodeRecipe.outputPinTypeIDs.size(), defaultPinNames);

		for (size_t i = 0; i < aDescription.inputPinNames.size(); ++i)
		{
			PinTypeManager::GetPinType(aNodeRecipe.inputPinTypeIDs[i]).name = aDescription.inputPinNames[i];
		}
		for (size_t i = 0; i < aDescription.outputPinNames.size(); ++i)
		{
			PinTypeManager::GetPinType(aNodeRecipe.outputPinTypeIDs[i]).name = aDescription.outputPinNames[i];
		}
		return NodeTypeManager::Register(NodeType{ std::move(aNodeRecipe), aNodeName });
	}

	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename OutputType, typename... InputTypes>
	inline NodeTypeID RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
	{
		return RegisterInternal(FilterNodeType<Traits, ExecutionTrait, OperatorTrait>(aFunction), aNodeName, aDescription);
	}

	template<typename T>
	inline T GetterNode(const InternalExecutionContext* aContext)
	{
		const NodeID nodeID = aContext->GetNodeData().currentNodeID;
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aContext->script);

		VarID varID = variableManager.GetVariableIDByNodeID(nodeID);

		MemoryPoolID runtimeID = ScriptProxy::GetVariable(aContext->script, varID).runtimeMemoryID;

		MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext->script);
		const T& output = memoryPool.At<T>(runtimeID);
		return output;
	}

	template<typename T>
	inline void SetterNode(const InternalExecutionContext* aContext, const T aValue)
	{
		const NodeID nodeID = aContext->GetNodeData().currentNodeID;
		VariableManager& variableManager = ScriptProxy::GetVariableManager(aContext->script);

		VarID varID = variableManager.GetVariableIDByNodeID(nodeID);

		MemoryPoolID runtimeID = ScriptProxy::GetVariable(aContext->script, varID).runtimeMemoryID;

		MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext->script);
		memoryPool.At<T>(runtimeID) = aValue;
	}

	template<typename T>/* requires IsValidScriptObjectType<T, nlohmann::json> || Fundamental<T>*/
	inline void RegisterGetterNodeType()
	{
		//NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Getter>(GetterNode<T>), "Get " + DataTypeManager::GetName(typeid(T).hash_code()));
		NodeTypeID nodeTypeID = RegisterInternal(CreateGetterNodeRecipe<T>(), "Get " + DataTypeManager::GetName(typeid(T).hash_code()));
		NodeTypeManager::SetGetterNodeTypeID(typeid(T).hash_code(), nodeTypeID);
	}


	template<typename T>/* requires IsValidScriptObjectType<T, nlohmann::json> || Fundamental<T>*/
	inline void RegisterSetterNodeType()
	{
		//NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Setter | eNodeTrait::HasImplicitFlow>(SetterNode<T>), "Set " + DataTypeManager::GetName(typeid(T).hash_code()));
		NodeTypeID nodeTypeID = RegisterInternal(CreateSetterNodeRecipe<T>(), "Set " + DataTypeManager::GetName(typeid(T).hash_code()));
		NodeTypeManager::SetSetterNodeTypeID(typeid(T).hash_code(), nodeTypeID);
	}

	class NodeTypeRegistry
	{
	public:

		template<eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, IsNotVoid OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterNodeType<eNodeTrait::None, ExecutionTrait>(aFunction), aNodeName, aDescription);
		}

		template<typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterFlowNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterNodeType<eNodeTrait::HasImplicitFlow>(aFunction), aNodeName, aDescription);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aDescription);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aDescription);
		}

		template<typename ClassType, typename MemberType>
		static void RegisterMemberVariable(MemberType ClassType::* aVariable, const std::string& aDirectory, const std::string& aVariableName)
		{
			auto getterFunc = [aVariable](ClassType* anObject) -> MemberType
				{
					if (!anObject)
					{
						return MemberType{};
					}
					return anObject->*aVariable;
				};
			;
			RegisterInternal(CreateNodeRecipe(getterFunc, TypeList<std::remove_const_t<MemberType>>(), TypeList<ClassType*>()), aDirectory + "Get " + aVariableName);

			if constexpr (!std::is_const_v<MemberType>)
			{ 
				auto setterFunc = [aVariable](Flow, ClassType* anObject, MemberType aValue) -> Flow
					{
						if (!anObject)
						{
							return true;
						}
						anObject->*aVariable = aValue;
						return true;
					};
				;
				RegisterInternal(CreateNodeRecipe(setterFunc, TypeList<Flow>(), TypeList<Flow, ClassType*, std::remove_const_t<MemberType>>()), aDirectory + "Set " + aVariableName);
			}
		}
	};
}