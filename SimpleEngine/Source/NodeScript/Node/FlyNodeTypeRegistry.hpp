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
#include "../DataType/FlyVariableRef.hpp"

namespace FLY_NAMESPACE
{

	inline std::string CleanUpNodeName(const std::string& aStr)
	{
		std::string result;
		for (size_t i = 0; i < aStr.size(); ++i)
		{
			if (i > 0 && std::isupper(aStr[i]) && std::islower(aStr[i - 1]))
			{
				result += ' ';
			}
			else if (i < aStr.size() - 1 && aStr[i] == ':' && aStr[i + 1] == ':')
			{
				result += '/' + result + ' ';
				i++;
				continue;
			}
			result += aStr[i];
		}
		return result;
	}

	inline std::string GetClassNameFromMemberName(const std::string& aStr)
	{
		return aStr.substr(0, aStr.find_first_of(':'));
	}

	inline NodeTypeID RegisterInternal(NodeRecipe&& aNodeRecipe, NodeTypeDesc&& aDescription = NodeTypeDesc())
	{
		const std::string defaultPinNames = aDescription.mShowDataTypePinNames ? TypeIdentifierStr : "";
		aDescription.mInputPinNames.resize(aNodeRecipe.mInputPinTypeIDs.size(), defaultPinNames);
		aDescription.mOutputPinNames.resize(aNodeRecipe.mOutputPinTypeIDs.size(), defaultPinNames);


		for (size_t i = 0; i < aDescription.mInputPinNames.size(); ++i)
		{
			Internal::GetPinTypeManager().GetPinType(aNodeRecipe.mInputPinTypeIDs[i]).mName = aDescription.mInputPinNames[i];
		}
		for (size_t i = 0; i < aDescription.mOutputPinNames.size(); ++i)
		{
			Internal::GetPinTypeManager().GetPinType(aNodeRecipe.mOutputPinTypeIDs[i]).mName = aDescription.mOutputPinNames[i];
		}
		aNodeRecipe.mName = CleanUpNodeName(aNodeRecipe.mName);

		assert(!aNodeRecipe.mName.empty());

		return Internal::GetNodeTypeManager().Register(NodeType{ .mNodeRecipe = std::move(aNodeRecipe) });
	}

	template<eNodeTrait Traits = eNodeTrait::None, typename OutputType, typename... InputTypes>
	inline NodeTypeID RegisterSystemNodeType(FuncPtr<OutputType, InputTypes...> aFunction, auto&& aCreationData = NodeCreationData())
	{
		return RegisterInternal(FilterNodeType<Traits>(aFunction, std::forward<NodeCreationData>(aCreationData)), std::forward<NodeTypeDesc>(aCreationData.mDescription));
	}

	template<typename T>
	inline T* GetterNode(InternalExecutionContextPtr aContext)
	{
		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));
		VariableInstance& variableInstance = aContext->mClassInstance->mVariableContainerInstance.mVariableInstances[variableRef.GetVarID()];
		T* outputValue = reinterpret_cast<T*>(variableInstance.mRuntimeValueDataPtr.Get());
		return outputValue;
	}

	template<typename T>
	inline void SetterNode(InternalExecutionContextPtr aContext, const T& aValue)
	{
		const VariableRef& variableRef = Internal::GetVariableRefByNodeRef(CreateGlobalNodeRef(aContext->mNodeData.mNodeRef, *aContext->mClass));
		VariableInstance& variableInstance = aContext->mClassInstance->mVariableContainerInstance.mVariableInstances[variableRef.GetVarID()];
		T& runtimeValue = *reinterpret_cast<T*>(variableInstance.mRuntimeValueDataPtr.Get());
		runtimeValue = aValue;
	}

	template<typename T>
	inline void RegisterGetterNodeType()
	{
		const std::string& typeName = Internal::GetDataTypeManager().GetName(GetDataTypeID<T>());
		NodeCreationData getterData;
		getterData.mName = "Get " + typeName;
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Getter>(GetterNode<T>, getterData));
		Internal::GetNodeTypeManager().SetGetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}


	template<typename T>
	inline void RegisterSetterNodeType()
	{
		const std::string& typeName = Internal::GetDataTypeManager().GetName(GetDataTypeID<T>());

		NodeCreationData setterData;
		setterData.mName = "Set " + typeName;
		const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<eNodeTrait::Setter | eNodeTrait::HasImplicitFlow>(SetterNode<T>, setterData));
		Internal::GetNodeTypeManager().SetSetterNodeTypeID(GetDataTypeID<T>(), nodeTypeID);
	}

	template<PointerType T>
	static T GetTargetNode(const InternalExecutionContext* aContext)
	{
		return reinterpret_cast<T>(aContext->mTarget);
	}


	class NodeTypeRegistry
	{
	public:

		template<eNodeTrait Traits, typename OutputType, typename... InputTypes>
		static void RegisterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, auto&& aCreationData = NodeCreationData())
		{
			RegisterInternal(FilterNodeType<Traits>(aFunction, std::forward<NodeCreationData>(aCreationData)), std::forward<NodeTypeDesc>(aCreationData.mDescription));
		}

		template<eNodeTrait Traits, typename ClassType, typename OutputType, typename... InputTypes>
		static void RegisterFakeMemberNodeType(FuncPtr<OutputType, InputTypes...> aFunction, auto&& aCreationData = NodeCreationData())
		{
			aCreationData.mOwnerDataTypeID = GetDataTypeID<ClassType*>();
			const NodeTypeID nodeTypeID = RegisterInternal(FilterNodeType<Traits>(aFunction, std::forward<NodeCreationData>(aCreationData)), std::forward<NodeTypeDesc>(aCreationData.mDescription));

			if (DataType* dataType = Internal::GetDataTypeManager().Find<ClassType*>())
			{
				dataType->mNodeTypeIDs.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename OutputType, typename... InputTypes>
		static void RegisterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, auto&& aCreationData = NodeCreationData())
		{
			aCreationData.mOwnerDataTypeID = GetDataTypeID<ClassType*>();
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction, std::forward<NodeCreationData>(aCreationData)), std::forward<NodeTypeDesc>(aCreationData.mDescription));

			if (DataType* dataType = Internal::GetDataTypeManager().Find<ClassType*>())
			{
				dataType->mNodeTypeIDs.push_back(nodeTypeID);
			}
		}

		template<typename ClassType, typename OutputType, typename... InputTypes>
		static void RegisterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, auto&& aCreationData = NodeCreationData())
		{
			aCreationData.mOwnerDataTypeID = GetDataTypeID<ClassType*>();
			const NodeTypeID nodeTypeID = RegisterInternal(FilterMemberNodeType(aFunction, std::forward<NodeCreationData>(aCreationData)), std::forward<NodeTypeDesc>(aCreationData.mDescription));

			if (DataType* dataType = Internal::GetDataTypeManager().Find<ClassType*>())
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


			NodeCreationData getterData;
			getterData.mName = aDirectory + "/Get " + aVariableName;
			getterData.mOwnerDataTypeID = GetDataTypeID<ClassType*>();

			const NodeTypeID getterNodeTypeID = RegisterInternal(
				CreateNodeRecipe(
					getterFunc,
					TypeList<std::remove_const_t<MemberType>>(),
					TypeList<ClassType*>(),
					std::move(getterData)
				)
			);

			DataType* dataType = Internal::GetDataTypeManager().Find<ClassType*>();
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

				NodeCreationData setterData;
				setterData.mName = aDirectory + "/Set " + aVariableName;
				setterData.mOwnerDataTypeID = GetDataTypeID<ClassType*>();
				const NodeTypeID setterNodeTypeID = RegisterInternal(
					CreateNodeRecipe(
						setterFunc,
						TypeList<Flow>(),
						TypeList<Flow, ClassType*, std::remove_const_t<MemberType>>(),
						std::move(setterData)
					)
				);

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
		constexpr InputNames(std::initializer_list<std::string> aNames)
			: mNames(aNames)
		{
		}
		std::vector<std::string> mNames;
	};

	struct OutputNames final
	{
		constexpr OutputNames(std::initializer_list<std::string> aNames)
			: mNames(aNames)
		{
		}
		std::vector<std::string> mNames;
	};

	struct DefaultValues final
	{

		constexpr DefaultValues(std::initializer_list<std::any> aValues)
			: mValues(aValues)
		{
		}
		std::vector<std::any> mValues;
	};

	template<typename... Args>
	struct DefaultValues2 final
	{
		constexpr DefaultValues2(Args&&... aDefaultValues)
			: mValues(std::tuple<Args...>{ std::forward<Args>(aDefaultValues)... })
		{

		}

		std::tuple<Args...> mValues;
	};

	template<typename... Args>
	DefaultValues2(Args&&...) -> DefaultValues2<Args...>;

	struct Directory final
	{
		std::string mDirectory;
	};

	template<typename T>
	struct MemberOf final
	{
		using Type = T;
	};

	// Helper to get the type at a given index in a parameter pack
	template<std::size_t Index, typename... Types>
	struct TypeAtIndex
	{
		// Use std::tuple_element to get the type
		using type = typename std::tuple_element<Index, std::tuple<Types...>>::type;
	};

	// Helper alias to simplify syntax
	template<std::size_t Index, typename... Types>
	using TypeAtIndex_t = typename TypeAtIndex<Index, Types...>::type;

	template<typename T>
	concept IsProtectedType = SameAsTemplateType<NodeState, std::decay_t<T>> ||
		std::same_as<InternalExecutionContextPtr, std::decay_t<T>> ||
		SameAsTemplateType<NodeExecutionContext, std::decay_t<T>>;

	template<size_t Index, typename T>
	struct TypeListElement;

	template<size_t Index, typename... Types>
	struct TypeListElement<Index, TypeList<Types...>>
	{
		using type = TypeAtIndex<Index, Types...>;
	};


	//template<typename OutputType, typename... InputTypes>
	//constexpr auto CreateTypeListFromFunction(FuncPtr<OutputType, InputTypes...>)
	//{
	//return TypeList<InputTypes...>{};
	//}

	//template<typename T, typename... Rest>
	//constexpr auto CreateTypeListWithoutProtectedTypesInternal()
	//{
	//	if constexpr (IsProtectedType<std::decay_t<T>>)
	//	{
	//		if constexpr (sizeof...(Rest) > 0)
	//		{
	//			return CreateTypeListWithoutProtectedTypesInternal<Rest...>();
	//		}
	//		else
	//		{
	//			return TypeList<None>{};
	//		}
	//	}
	//	else
	//	{
	//		return TypeList<TransformType_t<T>, TransformType_t<Rest>...>{};
	//	}
	//}

	//template<typename OutputType, typename... InputTypes>
	//constexpr auto CreateTypeListFromFunctionWithoutProtectedTypes(FuncPtr<OutputType, InputTypes...>)
	//{
	//	return CreateTypeListWithoutProtectedTypesInternal<InputTypes...>();
	//}

	//template<typename... Pack1, typename... Pack2>
	//constexpr std::tuple<Pack2...> ConvertTuple(const std::tuple<Pack1...>& inputTuple, std::tuple<Pack2...>* = nullptr)
	//{
	//	static_assert(sizeof...(Pack1) == sizeof...(Pack2), "Parameter packs must have the same size");

	//	// Helper lambda to unpack the tuple and transform types
	//	return[&]<std::size_t... Indices>(std::index_sequence<Indices...>)
	//	{
	//		return std::make_tuple(static_cast<Pack2>(std::get<Indices>(inputTuple))...);
	//	}(std::make_index_sequence<sizeof...(Pack1)>{});
	//}

	struct RegisterFunctionNode final
	{
		constexpr RegisterFunctionNode() = default;

		template<typename ContainerType, typename ContainerType2>
		constexpr static void InsertAtEnd(ContainerType& aContainer, ContainerType2&& aContainer2)
		{
			aContainer.insert(std::end(aContainer), std::begin(aContainer2), std::end(aContainer2));
		}

		//template<size_t CurrentIndex, typename T, typename... Rest>
		//constexpr static size_t GetFunctionParameterStartIndexInternal()
		//{
		//	if constexpr (IsProtectedType<T>)
		//	{
		//		if constexpr (sizeof...(Rest) > 0)
		//		{
		//			return GetFunctionParameterStartIndexInternal<CurrentIndex + 1, Rest...>();

		//		}
		//		else
		//		{
		//			return CurrentIndex;
		//		}
		//	}
		//	else
		//	{
		//		return CurrentIndex;
		//	}
		//}

		//template<typename... Args>
		//constexpr static size_t GetFunctionParameterStartIndex()
		//{
		//	return GetFunctionParameterStartIndexInternal<0, Args...>();
		//}


		//template<size_t CurrentIndex, size_t FuncParamStartIndex, typename Current, typename... Rest>
		//constexpr static void InsertStuffInternal(std::vector<std::any>& aAnyVector, const auto& aTuple, auto aTypeList)
		//{
		//	if constexpr (CurrentIndex < std::tuple_size_v<decltype(aTuple)>)
		//	{
		//		using RealType = std::decay_t<TypeListElement<decltype(aTypeList)>::type>;
		//		aAnyVector.emplace_back(RealType(std::get<CurrentIndex>(aTuple)));

		//		InsertStuffInternal<CurrentIndex + 1, FuncParamStartIndex, Rest...>(aAnyVector, aTuple, aTypeList);
		//	}
		//}

		//template<size_t StartIndex, typename... Args>
		//constexpr static void InsertStuff(std::vector<std::any>& aAnyVector, const auto& aTuple, auto aTypeList)
		//{
		//	constexpr size_t FuncParamStartIndex = 0;
		//	InsertStuffInternal<0, FuncParamStartIndex, Args...>(aAnyVector, aTuple, aTypeList);
		//}

		//template<typename... Pack1, typename... Pack2>
		//constexpr static void TryConvertTuple(const std::tuple<Pack1...>& aInputTuple, std::tuple<Pack2...>& aOutputTuple)
		//{
		//	static_assert(sizeof...(Pack1) == sizeof...(Pack2), "Parameter packs must have the same size");

		//	// Helper lambda to unpack the tuple and transform types
		//	return[&]<std::size_t... Indices>(std::index_sequence<Indices...>)
		//	{
		//		return std::make_tuple(static_cast<Pack2>(std::get<Indices>(aInputTuple))...);
		//	}(std::make_index_sequence<sizeof...(Pack1)>{});
		//}

		//template<typename... Pack1, typename... Pack2>
		//constexpr static void TryConvertTuple(const std::tuple<Pack1...>& aInputTuple, TypeList<Pack2...>, std::vector<std::any>& aOutputVector)
		//{
		//	std::tuple<TransformType_t<Pack2>...> outputTuple;
		//	TryConvertTuple(aInputTuple, outputTuple);
		//}

		template<typename ClassType = None, typename Function, typename... Extra>
		constexpr static auto CreateNodeCreationData(Function aFunction, const std::string& aFunctionName, [[maybe_unused]] Extra&&... aExtraTypes)
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

				InsertAtEnd(nodeCreationData.mDescription.mInputPinNames, std::move(Extract<InputNames>(std::forward<Extra>(aExtraTypes)...).mNames));
			}

			if constexpr (ContainsType<OutputNames, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mOutputPinNames.emplace_back(TypeIdentifierStr);
				}
				InsertAtEnd(nodeCreationData.mDescription.mOutputPinNames, std::move(Extract<OutputNames>(std::forward<Extra>(aExtraTypes)...).mNames));
			}

			if constexpr (ContainsType<DefaultValues, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mDefaultValues.emplace_back();
				}

				InsertAtEnd(nodeCreationData.mDescription.mDefaultValues, std::move(Extract<DefaultValues>(std::forward<Extra>(aExtraTypes)...).mValues));
			}

			if constexpr (ContainsTemplateType<DefaultValues2, Extra...>)
			{
				if constexpr (!isPure)
				{
					nodeCreationData.mDescription.mDefaultValues.emplace_back();
				}

				//const auto& defaultValuesTuple = ExtractTemplate<DefaultValues2>(std::forward<Extra>(aExtraTypes)...).mValues;

				//TryConvertTuple(defaultValuesTuple, CreateTypeListFromFunctionWithoutProtectedTypes(aFunction), nodeCreationData.mDescription.mDefaultValues);
			}


			if constexpr (ContainsType<AutoTick, Extra...>)
			{
				nodeCreationData.mEventID = AutoTickEventID;

				static_assert(!ContainsType<Event, Extra...>);
			}

			if constexpr (ContainsType<Directory, Extra...>)
			{
				nodeCreationData.mName = Extract<Directory>(std::forward<Extra>(aExtraTypes)...).mDirectory + "/" + aFunctionName;
			}
			else
			{
				if constexpr (!std::same_as<ClassType, None>)
				{
					const DataType* c = Internal::GetDataTypeManager().Find(GetDataTypeID<ClassType>());
					const DataType* classDataType = c ? c : Internal::GetDataTypeManager().Find<ClassType*>();
					assert(classDataType);
					nodeCreationData.mName = classDataType->mName + "/" + aFunctionName;
				}
				else
				{
					if (aFunctionName.find("::") == std::string::npos)
					{
						nodeCreationData.mName = "Default/" + aFunctionName;
					}
					else
					{
						nodeCreationData.mName = aFunctionName;
					}
				}
			}

			return nodeCreationData;
		}



		template<typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtr<OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			constexpr eNodeTrait Traits = ContainsType<Pure, Extra...> || ContainsType<Event, Extra...> ? eNodeTrait::None : eNodeTrait::HasImplicitFlow;

			if constexpr (ContainsTemplateType<MemberOf, Extra...>)
			{
				using ClassType = std::decay_t<decltype(ExtractTemplate<MemberOf, Extra...>(std::forward<Extra>(aExtraTypes)...))>::Type;
				NodeTypeRegistry::RegisterFakeMemberNodeType<Traits, ClassType>(aFunction, CreateNodeCreationData<ClassType>(aFunction, aFunctionName, std::forward<Extra>(aExtraTypes)...));
			}
			else
			{
				NodeTypeRegistry::RegisterNodeType<Traits>(aFunction, CreateNodeCreationData(aFunction, aFunctionName, std::forward<Extra>(aExtraTypes)...));
			}

			return RegisterFunctionNode();
		}


		template<typename ClassType, typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			//NodeTypeRegistry::RegisterMemberNodeType(aFunction, GetNodeCreationData<ClassType>(aFunction, aFunctionName, std::forward<Extra>(aExtraTypes)...));

			return RegisterFunctionNode();
		}

		template<typename ClassType, typename OutputType, typename... InputTypes, typename... Extra>
		constexpr static RegisterFunctionNode Register(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const std::string& aFunctionName, Extra&&... aExtraTypes)
		{
			//NodeTypeRegistry::RegisterMemberNodeType(aFunction, GetNodeCreationData<ClassType>(aFunction, aFunctionName, std::forward<Extra>(aExtraTypes)...));

			return RegisterFunctionNode();
		}
	};



}

#define FLY_FUNCTION(function, ...) \
    inline static FLY_NAMESPACE::RegisterFunctionNode FLY_UNIQUE_NAME(fly_function) = FLY_NAMESPACE::RegisterFunctionNode::Register(&function, #function, __VA_ARGS__);
