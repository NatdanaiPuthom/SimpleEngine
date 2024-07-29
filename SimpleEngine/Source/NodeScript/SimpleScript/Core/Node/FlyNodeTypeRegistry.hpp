#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "FlyNodeCreator.hpp"
#include "FlyNodeTypeManager.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../FlyClass.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "FlyExecutionTypes.hpp"
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
	inline NodeTypeID RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, const NodeCreationData& aCreationData = NodeCreationData())
	{
		return RegisterInternal(FilterNodeType<Traits>(aFunction, aCreationData), aNodeName, aCreationData.desc);
	}

	template<typename T>
	inline T GetterNode(const InternalExecutionContext* aContext)
	{
		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));

		const VariableInstance& variableInstance = aContext->mClassInstance->mVariableManagerInstance.mVariables[variableRef.GetVarID()];
		const T& output = *reinterpret_cast<const T*>(variableInstance.mRuntimeDataPtr.Get());
		return output;
	}

	template<typename T>
	inline void SetterNode(const InternalExecutionContext* aContext, const T& aValue)
	{

		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));

		VariableInstance& variableInstance = aContext->mClassInstance->mVariableManagerInstance.mVariables[variableRef.GetVarID()];

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
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, const NodeCreationData& aCreationData = NodeCreationData())
		{
			RegisterInternal(FilterNodeType(aFunction, aCreationData), aNodeName, aCreationData.desc);
		}

		template<typename OutputType, typename... InputTypes>
		static void RegisterFlowNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, const NodeCreationData& aCreationData = NodeCreationData())
		{
			//aCreationData.hasImplicitFlow = true;
			RegisterInternal(FilterNodeType<eNodeTrait::HasImplicitFlow>(aFunction, aCreationData), aNodeName, aCreationData.desc);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes> requires NoArgsReference<InputTypes...>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aCreationData.desc);

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
			NodeTypeID getterNodeTypeID = RegisterInternal(CreateNodeRecipe(getterFunc, TypeList<std::remove_const_t<MemberType>>(), TypeList<ClassType*>()), aDirectory + "Get " + aVariableName);

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
				NodeTypeID setterNodeTypeID = RegisterInternal(CreateNodeRecipe(setterFunc, TypeList<Flow>(), TypeList<Flow, ClassType*, std::remove_const_t<MemberType>>()), aDirectory + "Set " + aVariableName);

				if (dataType)
				{
					dataType->mFunctions.push_back(setterNodeTypeID);
				}
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

		constexpr static eNodeRegTrait UnpackTraits()
		{
			return eNodeRegTrait::None;
		}

		template<typename CurrentTrait, typename... Traits>
		constexpr static eNodeRegTrait UnpackTraits()
		{
			if constexpr (std::is_same_v<CurrentTrait, Event>)
			{
				return eNodeRegTrait::Event;
			}
			else
			{
				return eNodeRegTrait::None;
			}
		}

		volatile inline RegisterFunctionNode(FuncPtr<OutputType, InputTypes...> function, const std::string& functionName)
		{
			NodeCreationData nodeCreationData;
			NodeTypeRegistry::RegisterNodeType(function, functionName, nodeCreationData);
		}

		template<typename... Traits>
		volatile inline RegisterFunctionNode(FuncPtr<OutputType, InputTypes...> function, const std::string& functionName, TypeList<Traits...>)
		{
			eNodeRegTrait mTraits = eNodeRegTrait::None;
			if constexpr (sizeof...(Traits) > 0)
			{
				mTraits = UnpackTraits<Traits...>();
			}
			NodeCreationData nodeCreationData;
			if (HasFlag(mTraits, eNodeRegTrait::Event))
			{
				EventID mEventID = std::hash<decltype(function)>()(function);
				nodeCreationData.mEventID = mEventID;
			}
			NodeTypeRegistry::RegisterNodeType(function, functionName, nodeCreationData);
		}
	};



}
#define FLY_CONCATENATE_DETAIL(x, y) x##y
#define FLY_CONCATENATE(x, y) FLY_CONCATENATE_DETAIL(x, y)

// Macro to generate a unique name using __COUNTER__
#define FLY_UNIQUE_NAME(base) FLY_CONCATENATE(base, __COUNTER__)

#define REGISTER_FUNCTION(function, directory, ...) \
    inline static Fly::RegisterFunctionNode<decltype(&function)> FLY_UNIQUE_NAME(fly_function)(function, directory"/"#function, Fly::TypeList<__VA_ARGS__>());
