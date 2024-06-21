#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/ScriptUtilities.h"
#include "NodeCreator.h"
#include "NodeTypeManager.h"
#include "../Utilities/MetaScript.h"
#include "../Pin/PinTypeManager.h"
#include "../DataType/DataTypeManager.h"
#include "ScriptInstance.h"

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
		return NodeTypeManager::GetInstance().Register(NodeType{std::move(aNodeRecipe), aNodeName});
	}

	template<eNodeTrait Traits = eNodeTrait::None, eNodeEventType EventType = eNodeEventType::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename OutputType, typename... InputTypes>
	inline NodeTypeID RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
	{
		return RegisterInternal(FilterNodeType<Traits, EventType, OperatorTrait>(aFunction), aNodeName, aDescription);
	}

	template<typename T>
	inline T GetterNode(const InternalExecutionContext* aContext)
	{
		const NodeID nodeID = aContext->GetNodeData().nodeRef.nodeID;
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(*aContext->script);

		VarID varID = variableManager.GetVariableIDByNodeID(nodeID);

		//const void* runtimeDataPtr = ScriptProxy::GetVariable(*aContext->script, varID).runtimeDataPtr;
		const void* runtimeDataPtr = aContext->scriptInstance->myVariableManagerInstance.myVariables[varID].runtimeDataPtr;
		//MemoryPool& memoryPool = ScriptProxy::GetVariableMemoryPool(*aContext->script);
		const T& output = *reinterpret_cast<const T*>(runtimeDataPtr);
		return output;
	}

	template<typename T>
	inline void SetterNode(const InternalExecutionContext* aContext, const T& aValue)
	{
		const NodeID nodeID = aContext->GetNodeData().nodeRef.nodeID;
		VariableManager& variableManager = ScriptProxy::GetVariableManager(*aContext->script);

		VarID varID = variableManager.GetVariableIDByNodeID(nodeID);

		const VariableInstance& variableInstance = aContext->scriptInstance->myVariableManagerInstance.myVariables[varID];
		//const Variable& variable = ScriptProxy::GetVariable(*aContext->script, varID);
		
		T& runtimeValue = *reinterpret_cast<T*>(variableInstance.runtimeDataPtr);
		//T& runtimeValue = *reinterpret_cast<T*>(variable.runtimeDataPtr);
		runtimeValue = aValue;
	}

	template<typename T>/* requires IsValidScriptObjectType<T, nlohmann::json> || Fundamental<T>*/
	inline void RegisterGetterNodeType()
	{
		NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Getter>(GetterNode<T>), "Get " + Global::GetDataTypeManager().GetName(typeid(T).hash_code()));
		NodeTypeManager::GetInstance().SetGetterNodeTypeID(typeid(T).hash_code(), nodeTypeID);
	}


	template<typename T>/* requires IsValidScriptObjectType<T, nlohmann::json> || Fundamental<T>*/
	inline void RegisterSetterNodeType()
	{
		NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Setter | eNodeTrait::HasImplicitFlow>(SetterNode<T>), "Set " + Global::GetDataTypeManager().GetName(typeid(T).hash_code()));
		NodeTypeManager::GetInstance().SetSetterNodeTypeID(typeid(T).hash_code(), nodeTypeID);
	}

	template<typename T>
	static T* GetSelfNode(const InternalExecutionContext* aContext)
	{
		return (T*)aContext->owner;
	}


	class NodeTypeRegistry
	{
	public:

		template<eNodeEventType EventType = eNodeEventType::None, IsNotVoid OutputType, typename... InputTypes>
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterNodeType<eNodeTrait::None, EventType>(aFunction), aNodeName, aDescription);
		}

		template<typename OutputType, typename... InputTypes> /*requires NoArgsReference<InputTypes...>*/
		static void RegisterFlowNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			RegisterInternal(FilterNodeType<eNodeTrait::HasImplicitFlow>(aFunction), aNodeName, aDescription);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->functions.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->functions.push_back(nodeTypeID);
			}
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


	struct Event
	{

	};

	template<typename FunctionType>
	struct RegisterFunctionNode;


	enum class eNodeRegTrait : unsigned char
	{
		None = 0,
		Event = 1 << 0,
		Pure = 1 << 1
	};

	template<typename OutputType, typename... InputTypes>
	struct RegisterFunctionNode<FuncPtr<OutputType, InputTypes...>>
	{


		template<typename CurrentTrait, typename... Traits>
		constexpr static eNodeRegTrait UnpackTraits()
		{
			if constexpr (std::is_same_v<CurrentTrait, Event>)
			{
				return eNodeRegTrait::Event;
			}
			return eNodeRegTrait::None;
		}


		template<typename... Traits>
		volatile inline RegisterFunctionNode(FuncPtr<OutputType, InputTypes...> function, const std::string& functionName, TypeList<Traits...>)
		{
			constexpr eNodeRegTrait traits = UnpackTraits<Traits...>();
			if constexpr (HasFlag(traits, eNodeRegTrait::Event))
			{
				int a = 4;
				a;
			}
			SCRIPT::NodeTypeRegistry::RegisterNodeType(function, functionName);
		}
	};



}


#define REGISTER_FUNCTION(function, ...) \
    inline static SCRIPT::RegisterFunctionNode<decltype(&function)> __##function##RegisterFunctionNode(function, "Test/" #function, SCRIPT::TypeList<__VA_ARGS__>());
