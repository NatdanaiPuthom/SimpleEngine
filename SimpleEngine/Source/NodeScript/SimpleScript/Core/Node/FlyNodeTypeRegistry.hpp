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

	inline std::string InsertSpaces(const std::string& aStr)
	{
		std::string result;
		for (size_t i = 0; i < aStr.size(); ++i)
		{
			if (i > 0 && std::isupper(aStr[i]) && std::islower(aStr[i - 1]))
			{
				result += ' ';
			}
			result += aStr[i];
		}
		return result;
	}

	inline std::string GetClassFromMember(const std::string& aStr)
	{
		return aStr.substr(0, aStr.find_first_of(':'));
	}

	inline NodeTypeID RegisterInternal(NodeRecipe&& aNodeRecipe, const std::string& aNodeName, NodeTypeDesc aDescription = NodeTypeDesc())
	{
		const std::string defaultPinNames = aDescription.mShowDataTypePinNames ? TypeIdentifierStr : "";
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
		return Global::GetNodeTypeManager().Register(NodeType{ .mNodeRecipe = std::move(aNodeRecipe), .mName = InsertSpaces(aNodeName) });
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

		const VariableInstance& variableInstance = aContext->mClassInstance->mStructInstance.mVariableInstances[variableRef.GetVarID()];
		const T& output = *reinterpret_cast<const T*>(variableInstance.mRuntimeDataPtr.Get());
		return output;
	}

	template<typename T>
	inline void SetterNode(const InternalExecutionContext* aContext, const T& aValue)
	{

		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));

		VariableInstance& variableInstance = aContext->mClassInstance->mStructInstance.mVariableInstances[variableRef.GetVarID()];

		T& runtimeValue = *reinterpret_cast<T*>(variableInstance.mRuntimeDataPtr.Get());
		runtimeValue = aValue;
	}

	template<typename T>
	inline void RegisterGetterNodeType()
	{
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Getter>(GetterNode<T>, NodeCreationData{}), "Get " + Global::GetDataTypeManager().GetName(GetDataTypeID<T>()));
		Global::GetNodeTypeManager().SetGetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}


	template<typename T>
	inline void RegisterSetterNodeType()
	{
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Setter | eNodeTrait::HasImplicitFlow>(SetterNode<T>, NodeCreationData{}), "Set " + Global::GetDataTypeManager().GetName(GetDataTypeID<T>()));
		Global::GetNodeTypeManager().SetSetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}

	template<IsPointer T>
	static T GetSelfNode(const InternalExecutionContext* aContext)
	{
		return reinterpret_cast<T>(aContext->mTarget);
	}


	class NodeTypeRegistry
	{
	public:

		template<eNodeTrait Traits, typename OutputType, typename... InputTypes>
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			RegisterInternal(FilterNodeType<Traits>(aFunction, std::forward<NodeCreationData>(aCreationData)), aNodeName, aCreationData.mDescription);
		}

		template<typename ClassType, typename OutputType, typename... InputTypes>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aCreationData.mDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->mNodeTypeIDs.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename OutputType, typename... InputTypes>
		static void RegisterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aNodeName, NodeCreationData&& aCreationData = NodeCreationData())
		{
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction), aNodeName, aCreationData.mDescription);

			if (DataType* dataType = Global::GetDataTypeManager().Find<ClassType>())
			{
				dataType->mNodeTypeIDs.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename MemberType>
		static void RegisterMemberVariable(MemberType ClassType::* aMember, const std::string& aDirectory, const std::string& aVariableName)
		{
			auto getterFunc = [aMember](ClassType* anObject) -> MemberType
				{
					if (!anObject)
					{
						return MemberType{};
					}
					return anObject->*aMember;
				};

			const NodeTypeID getterNodeTypeID = RegisterInternal(CreateNodeRecipe(getterFunc, TypeList<std::remove_const_t<MemberType>>(), TypeList<ClassType*>(), NodeCreationData{ .mOwnerDataTypeID = GetDataTypeID<ClassType*>() }), aDirectory + "/Get " + aVariableName);

			DataType* dataType = Global::GetDataTypeManager().Find<ClassType*>();
			if (dataType)
			{
				dataType->mNodeTypeIDs.push_back(getterNodeTypeID);
			}

			if constexpr (!std::is_const_v<MemberType>)
			{
				auto setterFunc = [aMember](Flow, ClassType* anObject, const MemberType& aValue) -> Flow
					{
						if (anObject)
						{
							anObject->*aMember = aValue;
						}
						return Flow(true);
					};
				;
				const NodeTypeID setterNodeTypeID = RegisterInternal(CreateNodeRecipe(setterFunc, TypeList<Flow>(), TypeList<Flow, ClassType*, std::remove_const_t<MemberType>>(), NodeCreationData{ .mOwnerDataTypeID = GetDataTypeID<ClassType*>() }), aDirectory + "/Set " + aVariableName);

				if (dataType)
				{
					dataType->mNodeTypeIDs.push_back(setterNodeTypeID);
				}
			}
		}
	};

	struct Event final
	{
	};

	struct AutoTick final
	{
	};

	struct Pure final
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
		constexpr RegisterFunctionNode() = default;

		template<typename T, typename First, typename... Rest>
		constexpr static T&& Extract(First&& first, [[maybe_unused]] Rest&&... rest)
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


		template<typename ContainerType, typename ContainerType2>
		constexpr static void InsertAtEnd(ContainerType& aContainer, ContainerType2&& aContainer2)
		{
			aContainer.insert(std::end(aContainer), std::begin(aContainer2), std::end(aContainer2));
		}

		template<typename Function, typename... Extra>
		constexpr static NodeCreationData GetNodeCreationData(Function aFunction, [[maybe_unused]] Extra&&... aExtraTypes)
		{
			constexpr bool isPure = ContainsType<Pure, Extra...>;
			NodeCreationData nodeCreationData;
			if constexpr (ContainsType<Event, Extra...>)
			{
				const EventID eventID = std::hash<Function>()(aFunction);
				nodeCreationData.mEventID = eventID;
			}

			if constexpr (ContainsType<InputNames, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mInputPinNames.emplace_back(TypeIdentifierStr);
				}

				InsertAtEnd(nodeCreationData.mDescription.mInputPinNames, std::move(Extract<InputNames, Extra...>(std::forward<Extra>(aExtraTypes)...).mNames));
			}

			if constexpr (ContainsType<OutputNames, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mOutputPinNames.emplace_back(TypeIdentifierStr);
				}
				InsertAtEnd(nodeCreationData.mDescription.mOutputPinNames, std::move(Extract<OutputNames, Extra...>(std::forward<Extra>(aExtraTypes)...).mNames));
			}

			if constexpr (ContainsType<DefaultValues, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mDefaultValues.emplace_back();
				}

				InsertAtEnd(nodeCreationData.mDescription.mDefaultValues, std::move(Extract<DefaultValues, Extra...>(std::forward<Extra>(aExtraTypes)...).mValues));
			}

			if constexpr (ContainsType<AutoTick, Extra...>)
			{
				nodeCreationData.mEventID = AutoTickEventID;

				static_assert(!ContainsType<Event, Extra...>);
			}

			return nodeCreationData;
		}

		template<typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			constexpr eNodeTrait Traits = ContainsType<Pure, Extra...> || ContainsType<Event, Extra...> ? eNodeTrait::None : eNodeTrait::HasImplicitFlow;

			NodeTypeRegistry::RegisterNodeType<Traits>(aFunction, aFunctionName, GetNodeCreationData(aFunction, std::forward<Extra>(aExtraTypes)...));

			return RegisterFunctionNode();
		}


		template<typename ClassType, typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			NodeTypeRegistry::RegisterMemberNodeType(aFunction, aFunctionName, GetNodeCreationData(aFunction, std::forward<Extra>(aExtraTypes)...));

			return RegisterFunctionNode();
		}

		template<typename ClassType, typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			NodeTypeRegistry::RegisterMemberNodeType(aFunction, aFunctionName, GetNodeCreationData(aFunction, std::forward<Extra>(aExtraTypes)...));

			return RegisterFunctionNode();
		}
	};



}
#define FLY_CONCATENATE_DETAIL(x, y) x##y
#define FLY_CONCATENATE(x, y) FLY_CONCATENATE_DETAIL(x, y)

// Macro to generate a unique name using __COUNTER__
#define FLY_UNIQUE_NAME(base) FLY_CONCATENATE(base, __COUNTER__)

#define FLY_FUNCTION(function, directory, ...) \
    inline static Fly::RegisterFunctionNode FLY_UNIQUE_NAME(fly_function) = Fly::RegisterFunctionNode::Register(&function, directory"/"#function, __VA_ARGS__);
