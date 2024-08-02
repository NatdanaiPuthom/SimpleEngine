#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "FlyNodeCreator.hpp"
#include "FlyNodeTypeManager.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../Variable/FlyVariableRef.hpp"

namespace FLY_NAMESPACE
{
	inline NodeTypeID RegisterInternal(NodeRecipe&& aNodeRecipe, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
	{
		const std::string defaultPinNames = aDescription.mShowDataTypePinNames ? "#T" : "";
		aDescription.mInputPinNames.resize(aNodeRecipe.mInputPinTypeIDs.size(), defaultPinNames);
		aDescription.mOutputPinNames.resize(aNodeRecipe.mOutputPinTypeIDs.size(), defaultPinNames);


		for (size_t i = 0; i < aDescription.mInputPinNames.size(); ++i)
		{
			Global::GetPinTypeManager().GetPinType(aNodeRecipe.mInputPinTypeIDs[i]).mName = aDescription.mInputPinNames[i];
		}
		for (size_t i = 0; i < aDescription.mOutputPinNames.size(); ++i)
		{
			Global::GetPinTypeManager().GetPinType(aNodeRecipe.mOutputPinTypeIDs[i]).mName = aDescription.mOutputPinNames[i];
		}
		return Global::GetNodeTypeManager().Register(NodeType{std::move(aNodeRecipe), aNodeName});
	}

	template<eNodeTrait Traits = eNodeTrait::None, typename OutputType, typename... InputTypes>
	inline NodeTypeID RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
	{
		return RegisterInternal(FilterNodeType<Traits>(aFunction, std::forward<NodeCreationData>(aCreationData)), aNodeName, aCreationData.mDescription);
	}

	template<typename T>
	inline T GetterNode(const InternalExecutionContext* aContext)
	{
		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));

		const VariableInstance& variableInstance = aContext->mClassInstance->mStructInstance.mVariables[variableRef.GetVarID()];
		const T& output = *reinterpret_cast<const T*>(variableInstance.mRuntimeDataPtr.Get());
		return output;
	}

	template<typename T>
	inline void SetterNode(const InternalExecutionContext* aContext, const T& aValue)
	{

		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));

		VariableInstance& variableInstance = aContext->mClassInstance->mStructInstance.mVariables[variableRef.GetVarID()];

		T& runtimeValue = *reinterpret_cast<T*>(variableInstance.mRuntimeDataPtr.Get());
		runtimeValue = aValue;
	}

	template<typename T>
	inline void RegisterGetterNodeType()
	{
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Getter>(GetterNode<T>, NodeCreationData{}), "Get " + Global::GetDataTypeManager().GetName(typeid(T).hash_code()));
		Global::GetNodeTypeManager().SetGetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}


	template<typename T>
	inline void RegisterSetterNodeType()
	{
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Setter | eNodeTrait::HasImplicitFlow>(SetterNode<T>, NodeCreationData{ /*.hasImplicitFlow = true*/ }), "Set " + Global::GetDataTypeManager().GetName(typeid(T).hash_code()));
		Global::GetNodeTypeManager().SetSetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}

	template<typename T>
	static T* GetSelfNode(const InternalExecutionContext* aContext)
	{
		return reinterpret_cast<T*>(aContext->mOwner);
	}


	class NodeTypeRegistry
	{
	public:

		template<IsNotVoid OutputType, typename... InputTypes>
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			RegisterInternal(FilterNodeType(aFunction, std::forward<NodeCreationData>(aCreationData)), aNodeName, aCreationData.mDescription);
		}

		template<typename OutputType, typename... InputTypes>
		static void RegisterFlowNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			//aCreationData.hasImplicitFlow = true;
			RegisterInternal(FilterNodeType<eNodeTrait::HasImplicitFlow>(aFunction, std::forward<NodeCreationData>(aCreationData)), aNodeName, aCreationData.mDescription);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aCreationData.mDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->mFunctions.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->mFunctions.push_back(nodeTypeID);
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
			const NodeTypeID getterNodeTypeID = RegisterInternal(CreateNodeRecipe(getterFunc, TypeList<std::remove_const_t<MemberType>>(), TypeList<ClassType*>()), aDirectory + "Get " + aVariableName);

			DataType* dataType = Global::GetDataTypeManager().Find<ClassType>();
			if (dataType)
			{
				dataType->mFunctions.push_back(getterNodeTypeID);
			}

			if constexpr (!std::is_const_v<MemberType>)
			{
				auto setterFunc = [aVariable](Flow, ClassType* anObject, const MemberType& aValue) -> Flow
					{
						if (anObject)
						{
							anObject->*aVariable = aValue;
						}
						return true;
					};
				;
				const NodeTypeID setterNodeTypeID = RegisterInternal(CreateNodeRecipe(setterFunc, TypeList<Flow>(), TypeList<Flow, ClassType*, std::remove_const_t<MemberType>>()), aDirectory + "Set " + aVariableName);

				if (dataType)
				{
					dataType->mFunctions.push_back(setterNodeTypeID);
				}
			}
		}
	};

	struct Event final
	{
	};

	struct InputNames final
	{
		InputNames(std::initializer_list<std::string> aNames)
			: mNames(aNames)
		{
		}
		std::vector<std::string> mNames;
	};

	struct OutputNames final
	{
		OutputNames(std::initializer_list<std::string> aNames)
			: mNames(aNames)
		{
		}
		std::vector<std::string> mNames;
	};

	struct DefaultValues final
	{
		DefaultValues(std::initializer_list<std::any> aValues)
			: mValues(aValues)
		{
		}
		std::vector<std::any> mValues;
	};

	enum class eNodeRegTrait : unsigned char
	{
		None = 0,
		Event = 1 << 0,
		Pure = 1 << 1
	};

	struct RegisterFunctionNode final
	{

		template<typename T, typename First, typename... Rest>
		static T&& Extract(First&& first, [[maybe_unused]] Rest&&... rest)
		{
			if constexpr (std::is_same_v<T, First>) 
			{
				return std::forward<First>(first);
			}
			else 
			{
				return Extract<T>(std::forward<Rest>(rest)...);
			}
		}

		template<typename OutputType, typename... InputTypes, typename... Extra>
		static RegisterFunctionNode Register(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aFunctionName, [[maybe_unused]] Extra&&... aExtraTypes)
		{
			NodeCreationData nodeCreationData;
			if constexpr (ContainsType<Event, Extra...>)
			{
				const EventID eventID = std::hash<decltype(aFunction)>()(aFunction);
				nodeCreationData.mEventID = eventID;
			}

			if constexpr (ContainsType<InputNames, Extra...>)
			{
				nodeCreationData.mDescription.mInputPinNames = Extract<InputNames>(std::forward<Extra>(aExtraTypes)...).mNames;
			}

			if constexpr (ContainsType<OutputNames, Extra...>)
			{
				nodeCreationData.mDescription.mOutputPinNames = Extract<OutputNames>(std::forward<Extra>(aExtraTypes)...).mNames;
			}

			if constexpr (ContainsType<DefaultValues, Extra...>)
			{
				nodeCreationData.mDescription.mDefaultValues = Extract<DefaultValues>(std::forward<Extra>(aExtraTypes)...).mValues;
			}

			NodeTypeRegistry::RegisterNodeType(aFunction, aFunctionName, std::move(nodeCreationData));

			return RegisterFunctionNode();
		}
	};



}
#define FLY_CONCATENATE_DETAIL(x, y) x##y
#define FLY_CONCATENATE(x, y) FLY_CONCATENATE_DETAIL(x, y)

// Macro to generate a unique name using __COUNTER__
#define FLY_UNIQUE_NAME(base) FLY_CONCATENATE(base, __COUNTER__)

#define REGISTER_FUNCTION(function, directory, ...) \
    inline static Fly::RegisterFunctionNode FLY_UNIQUE_NAME(fly_function) = Fly::RegisterFunctionNode::Register(function, directory"/"#function, __VA_ARGS__);
