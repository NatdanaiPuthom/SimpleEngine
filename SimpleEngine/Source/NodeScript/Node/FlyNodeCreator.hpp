#pragma once
#include "../FlyDefines.hpp"
#include "../Execution/FlyExecutionTypes.hpp"
#include "../Execution/FlyInternalExecutionContext.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Execution/FlyNodeExecutor.hpp"
#include "../Execution/FlyNodeExecutionQueue.hpp"
#include "FlyNode.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../SystemTypes/FlyWildcard.hpp"
#include "../SystemTypes/FlyFlow.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../SystemTypes/FlyReferenceWrapper.hpp"
#include "../Execution/FlyExecuteNode.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Utilities/FlyUtilities.hpp"

namespace FLY_NAMESPACE
{
	
	struct NodeCreationData final
	{
		EventID mEventID = InvalidID<EventID>();
		eNodeOperatorTrait mOperatorTrait = eNodeOperatorTrait::None;
		DataTypeID mOwnerDataTypeID = InvalidID<DataTypeID>();
		std::string mName;
		NodeTypeDesc mDescription;
	};

	using InternalExecutionContextPtr = InternalExecutionContext*;

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, NodeGraph& aDestinationNodeGraph, const NodeGraph& aSourceNodeGraph, const size_t aStartIndex);

	template<Decayed T, eFlowType FlowType>
	SetPinValueInterface CreateSetPinValueInterface()
	{
		return [](const SetPinValueData& aSetPinValueData, [[maybe_unused]] const InternalExecutionContext& aContext) -> void
			{
				//const Pin& readFromPin = aSetPinValueData.mNodeGraph->mPins[aSetPinValueData.mReadFromPinID];
				Pin& writeToPin = aSetPinValueData.mNodeGraph->mPins[aSetPinValueData.mWriteToPinID];
				//const PinType& readFromPinType = Global::GetPinTypeManager().GetPinType(readFromPin.mTypeID);
				[[maybe_unused]] const PinType& writeToPinType = Global::GetPinTypeManager().GetPinType(writeToPin.mTypeID);
#ifdef FLY_DEBUG
				//assert(readFromPinType.mDataTypeID == GetDataTypeID<T>());
				assert(writeToPinType.mDataTypeID == GetDataTypeID<T>());
#endif



				const T& readFromValue = *reinterpret_cast<const T*>(aSetPinValueData.mReadFromDataPtr);

				T& writeToValue = *reinterpret_cast<T*>(writeToPin.mDataPtr.Get());
				writeToValue = readFromValue;

				//const PinType& readFromPinType = Global::GetPinTypeManager().GetPinType(aSetPinData.)

				/*if (readFromPinType.isPointer)
				{

					const T* readFromValue = reinterpret_cast<const T*>(aSetPinData.mValue);
					if (writeToPin.isValue)
					{
						writeToValue = *readFromValue;
					}
				}*/

				if constexpr (std::same_as<T, Flow>)
				{
					if (writeToValue)
					{
						if constexpr (FlowType == eFlowType::Output)
						{
							for (const PinID connectedInputPinID : writeToPin.mConnectedPinIDs)
							{
								const Pin& connectedInputPin = aSetPinValueData.mNodeGraph->mPins[connectedInputPinID];
								aContext.mExecutionQueue->Push(NodeExecutionData{ NodeRef(connectedInputPin.mNodeID, *aSetPinValueData.mNodeGraph), eNodeTriggerReason::Flow });
								
#ifdef FLY_DEBUG
								const LinkID linkID = Internal::GetLinkIDByPinIDs(*aSetPinValueData.mNodeGraph, connectedInputPinID, aSetPinValueData.mWriteToPinID);
								aContext.mNodeExecutor->GetDebugger().AddTraversedLink(linkID, aContext.mNodeGraphVariantHandle);
#endif
							
							}
						}
					}
				}
			};
	}

	template<Decayed T, eFlowType FlowType>
	SetPinValueFromPinInterface CreateSetPinValueFromPinInterface()
	{
		return [](const SetPinValueFromPinData& aSetPinValueData, [[maybe_unused]] const InternalExecutionContext& aContext) -> void
			{
				const Pin& readFromPin = aSetPinValueData.mNodeGraph->mPins[aSetPinValueData.mReadFromPinID];
				Pin& writeToPin = aSetPinValueData.mNodeGraph->mPins[aSetPinValueData.mWriteToPinID];
				const PinType& readFromPinType = Global::GetPinTypeManager().GetPinType(readFromPin.mTypeID);
				const PinType& writeToPinType = Global::GetPinTypeManager().GetPinType(writeToPin.mTypeID);


				//assert(readFromPinType.mDataTypeID == GetDataTypeID<T>());
				assert(writeToPinType.mDataTypeID == GetDataTypeID<T>());

				const T& readFromValue = *reinterpret_cast<const T*>(readFromPin.mDataPtr.Get());

				T& writeToValue = *reinterpret_cast<T*>(writeToPin.mDataPtr.Get());

				if (HasFlag(Global::GetDataTypeManager().Find(readFromPinType.mDataTypeID)->mTypeTraits, eDataTypeTrait::Pointer))
				{
					if constexpr (!PointerType<T>)
					{
						// Since ReadFromPin is pointer -> ReadFromPin.mDataPtr is actually T**
						//const T* read = reinterpret_cast<const T*>(readFromPin.mDataPtr.Get());
						//const T** read2 = ((const T**)(const T*)readFromPin.mDataPtr.Get());
						writeToValue = **((const T**)((const T*)readFromPin.mDataPtr.Get()));
					}
					else
					{
						writeToValue = readFromValue;
					}
				}
				else
				{
					if constexpr (!PointerType<T>)
					{
						writeToValue = readFromValue;
					}
					else
					{
						using RemovePtrType = std::remove_pointer_t<T>;
						/*RemovePtrType* r = (RemovePtrType*)readFromValue;
						r;*/
						 RemovePtrType* const* r1 = reinterpret_cast<const T*>(readFromPin.mDataPtr.Get());
						 RemovePtrType* r = (RemovePtrType*)r1;
						 //r1;
						 r;
						writeToValue = r;
					}
				}

				if constexpr (std::same_as<T, Flow>)
				{
					if (writeToValue)
					{
						if constexpr (FlowType == eFlowType::Output)
						{
							for (const PinID connectedInputPinID : writeToPin.mConnectedPinIDs)
							{
								const Pin& connectedInputPin = aSetPinValueData.mNodeGraph->mPins[connectedInputPinID];
								aContext.mExecutionQueue->Push(NodeExecutionData{ NodeRef(connectedInputPin.mNodeID, *aSetPinValueData.mNodeGraph), eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	// Define the type transformation logic
	template <typename T>
	struct TransformType
	{
		using type = std::decay_t<T>; // Default to T
	};

	// Specialization: const T& -> T
	template <typename T>
	struct TransformType<const T&>
	{
		using type = T;
	};

	// Specialization: T& -> T*
	template <typename T>
	struct TransformType<T&>
	{
		//static_assert()
		using type = T&;
	};

	// Specialization: T* -> T* (already handled by default, but shown for clarity)
	template <typename T>
	struct TransformType<T*>
	{
		using type = T*;
	};

	// Helper alias template for convenience
	template <typename T>
	using TransformType_t = typename TransformType<T>::type;

	template<size_t Index, size_t Size, eFlowType FlowType, typename Type, typename... Types>
	constexpr void CreatePinTypesInternal(auto& aPinTypeIDs, const NodeCreationData& aCreationData)
	{
		static_assert(!std::is_reference_v<Type>);
		if constexpr (Index < Size)
		{
			using AllocationType = TransformType_t<Type>;

			const std::vector<std::string>& pinNames = SelectByFlowType(FlowType, aCreationData.mDescription.mInputPinNames, aCreationData.mDescription.mOutputPinNames);

			MemoryPoolID defaultValueMemoryID = InvalidID<MemoryPoolID>();
			if constexpr (FlowType == eFlowType::Input)
			{
				const std::any& defaultValueAny = aCreationData.mDescription.mDefaultValues.at(Index);
				if (defaultValueAny.has_value())
				{
					defaultValueMemoryID = Global::Internal::GetMemoryPool().Allocate<AllocationType>(std::any_cast<AllocationType>(defaultValueAny));
				}
			}
			aPinTypeIDs[Index] = Global::GetPinTypeManager().CreatePinType<AllocationType>(pinNames[Index], FlowType, CreateSetPinValueInterface<AllocationType, FlowType>(), CreateSetPinValueFromPinInterface<AllocationType, FlowType>(), defaultValueMemoryID);

			if constexpr (Index + 1 < Size)
			{
				CreatePinTypesInternal<Index + 1, Size, FlowType, Types...>(aPinTypeIDs, aCreationData);
			}
		}
	}

	template<eFlowType FlowType, typename... Types>
	constexpr std::vector<PinTypeID> CreatePinTypes(const NodeCreationData& aCreationData)
	{
		constexpr size_t Size = sizeof...(Types);
		std::vector<PinTypeID> pinTypeIDs(Size);

		if constexpr (!EmptyParameterPack<Types...>)
		{
			CreatePinTypesInternal<0, Size, FlowType, Types...>(pinTypeIDs, aCreationData);
		}
		return pinTypeIDs;
	}

	// Creates an input for a node instance
	template<typename InputType>
	PinID CreateInputPin(const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t anIndex, NodeGraph& aNodeGraph)
	{
		static_assert(!(std::is_reference_v<InputType> && !std::is_const_v<std::remove_reference_t<InputType>>), "Non const references are not supported");

		using DecayedType = std::decay_t<InputType>;

		const PinTypeID pinTypeID = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID).mNodeRecipe.mInputPinTypeIDs[anIndex];

		void* const dataPtr = [pinTypeID, &aNodeGraph]() -> void*
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pinTypeID);
				MemoryArena<NodeGraphBufferSize>& memoryArena = aNodeGraph.mMemoryArena;
				if (pinType.mDefaultValueID != InvalidID<MemoryPoolID>())
				{
					const MemoryPool& globalMemoryPool = Global::Internal::GetMemoryPool();
					return &memoryArena.Allocate<DecayedType>(globalMemoryPool.At<InputType>(pinType.mDefaultValueID));
				}
				else
				{
					return &memoryArena.Allocate<DecayedType>();
				}
			}();

		return Internal::CreatePin(aNodeGraph, aNodeID, pinTypeID, dataPtr);

	}
	template<size_t Index, size_t InputSize, typename InputType, typename... InputTypes>
	void CreateInputPinsInternal(const NodeID aNodeID, const NodeTypeID aNodeTypeID, std::array<PinID, InputSize>& aPinIDs, NodeGraph& aNodeGraph)
	{
		aPinIDs[Index] = CreateInputPin<InputType>(aNodeID, aNodeTypeID, Index, aNodeGraph);
		if constexpr (!EmptyParameterPack<InputTypes...>)
		{
			CreateInputPinsInternal<Index + 1, InputSize, InputTypes...>(aNodeID, aNodeTypeID, aPinIDs, aNodeGraph);
		}
	}

	template<typename... Inputs>
	std::array<PinID, sizeof...(Inputs)> CreateInputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, NodeGraph& aNodeGraph)
	{
		constexpr size_t InputSize = sizeof...(Inputs);
		std::array<PinID, InputSize> pinIDs{};
		if constexpr (!EmptyParameterPack<Inputs...>)
		{
			CreateInputPinsInternal<0, InputSize, Inputs...>(aNodeID, aNodeTypeID, pinIDs, aNodeGraph);
		}
		return pinIDs;
	}


	template<typename OutputType>
	PinID CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, NodeGraph& aNodeGraph)
	{
		static_assert(!std::is_reference_v<OutputType>, "Return type can't be a reference");
		static_assert(!std::is_same_v<void, OutputType>, "Return type can't be void");

		void* const dataPtr = &aNodeGraph.mMemoryArena.Allocate<OutputType>();
		return Internal::CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
	}

	template<size_t Index, size_t OutputSize, typename OutputType, typename... OutputTypes>
	void CreateOutputPinsInternal(const NodeID aNodeID, const NodeTypeID aNodeTypeID, std::array<PinID, OutputSize>& aPinIDs, NodeGraph& aNodeGraph)
	{
		if constexpr (Index < OutputSize)
		{

			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const PinTypeID pinTypeID = nodeType.mNodeRecipe.mOutputPinTypeIDs.at(Index);

			aPinIDs[Index] = CreateOutputPin<OutputType>(aNodeID, pinTypeID, aNodeGraph);

			if constexpr (Index + 1 < OutputSize)
			{
				CreateOutputPinsInternal<Index + 1, OutputSize, OutputTypes...>(aNodeID, aNodeTypeID, aPinIDs, aNodeGraph);
			}
		}
	}

	template<typename... OutputTypes>
	std::array<PinID, sizeof...(OutputTypes)> CreateOutputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, NodeGraph& aNodeGraph)
	{
		std::array<PinID, sizeof...(OutputTypes)> pinIDs{};
		if constexpr (!EmptyParameterPack<OutputTypes...>)
		{
			CreateOutputPinsInternal<0, sizeof...(OutputTypes), OutputTypes...>(aNodeID, aNodeTypeID, pinIDs, aNodeGraph);
		}
		return pinIDs;
	}

	// Function definitions.
	template<typename TupleType, size_t Index, typename Arg, typename... Args>
	void CreateInputPackInternal(TupleType& aTuple, const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		if constexpr (Index < std::tuple_size_v<TupleType>)
		{
			Pin& pin = aContext.mNodeData.mNodeRef.GetNodeGraph().mPins[aPinIDs[Index]];
			std::get<Index>(aTuple) = ReferenceWrapper<Arg>(*reinterpret_cast<Arg*>(pin.mDataPtr.Get()));

			if constexpr (sizeof...(Args) > 0)
			{
				CreateInputPackInternal<TupleType, Index + 1, Args...>(aTuple, aPinIDs, aContext);
			}
		}
	}

	template<typename... Args>
	std::tuple<ReferenceWrapper<Args>...> CreateInputPack(const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		std::tuple<ReferenceWrapper<Args>...> tuple;
		if constexpr (sizeof...(Args) > 0)
		{
			CreateInputPackInternal<std::tuple<ReferenceWrapper<Args>...>, 0, Args...>(tuple, aPinIDs, aContext);
		}
		return tuple;
	}

	template<typename Return, typename A, typename B>
	Return AppendContainers(A&& a, B&& b)
	{
		Return r{};
		r.insert(begin(r), begin(a), end(a));
		r.insert(end(r), begin(b), end(b));
		return r;
	}

	template<typename... OutputTypes, typename... InputTypes>
	CreateNodeSignature CreateCreateNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{
		return [](const NodeID aNodeID, const NodeTypeID aNodeTypeID, NodeGraph& aNodeGraph) -> Node
			{

				constexpr size_t PinInputSize = sizeof...(InputTypes);
				constexpr size_t PinOutputSize = sizeof...(OutputTypes);

				// Create Node Function

				// Input pins
				std::array<PinID, PinInputSize> preExistingInputPinIDs = CreateInputPins<InputTypes...>(aNodeID, aNodeTypeID, aNodeGraph);

				std::vector<PinID> addedInputPinIDs = Internal::CreateInputPins(aNodeGraph, aNodeID, aNodeTypeID, preExistingInputPinIDs.size());

				const std::vector<PinID> totalInputPinIDs = AppendContainers<std::vector<PinID>>(std::move(preExistingInputPinIDs), std::move(addedInputPinIDs));

				// Output pins
				std::array<PinID, PinOutputSize> preExistingOutputPinIDs = CreateOutputPins<OutputTypes...>(aNodeID, aNodeTypeID, aNodeGraph);

				std::vector<PinID> addedOutputPinIDs = Internal::CreateOutputPins(aNodeGraph, aNodeID, aNodeTypeID, preExistingOutputPinIDs.size());

				const std::vector<PinID> totalOutputPinIDs = AppendContainers<std::vector<PinID>>(std::move(preExistingOutputPinIDs), std::move(addedOutputPinIDs));

				return Node(aNodeTypeID, totalInputPinIDs, totalOutputPinIDs);
			};

	}

	template<eNodeTrait Traits = eNodeTrait::None, typename NodeExecutionContextType = Wildcard, typename NodeStateDataType = Wildcard, typename Callable, typename... OutputTypes, typename... InputTypes>
	NodeRecipe CreateNodeRecipe(Callable aCallable, TypeList<OutputTypes...> aOutputList, TypeList<InputTypes...>, NodeCreationData aCreationData)
	{
		static_assert(!AnyArgIsRawReference<InputTypes...> && "Function parameter cannot be of reference type. Consider adding const or change to pointer");

		constexpr bool TakesExecutionContext = HasFlag(Traits, eNodeTrait::TakesExecutionContext);
		constexpr bool TakesNodeState = !std::same_as<Wildcard, NodeStateDataType>;
		constexpr bool TakesInternalExecutionContext = HasFlag(Traits, eNodeTrait::TakesInternalExecutionContext);
		constexpr bool HasFlow = ContainsType<Flow, OutputTypes...> || ContainsType<Flow, InputTypes...>;

		DataTypeID nodeStateDataTypeID = InvalidID<DataTypeID>();
		if constexpr (TakesNodeState)
		{
			Global::GetDataTypeManager().Register<NodeStateDataType>(typeid(NodeStateDataType).name(), false);
			nodeStateDataTypeID = GetDataTypeID<NodeStateDataType>();
		}

		eNodeTrait traits = Traits;
		if constexpr (HasFlow)
		{
			traits |= eNodeTrait::HasFlow;
		}
		aCreationData.mDescription.mInputPinNames.resize(sizeof...(InputTypes), TypeIdentifierStr);
		aCreationData.mDescription.mOutputPinNames.resize(sizeof...(OutputTypes), TypeIdentifierStr);
		aCreationData.mDescription.mDefaultValues.resize(sizeof...(InputTypes));

		const std::vector<PinTypeID> inputPinTypeIDs = CreatePinTypes<eFlowType::Input, TransformType_t<InputTypes>...>(aCreationData);
		const std::vector<PinTypeID> outputPinTypeIDs = CreatePinTypes<eFlowType::Output, OutputTypes...>(aCreationData);

		MemoryPool& foundationMemoryPool = Global::Internal::GetMemoryPool();
		const MemoryPoolID functionMemoryID = foundationMemoryPool.Allocate<Callable>(aCallable);


		return NodeRecipe
		{
			.mCreateFunction = CreateCreateNodeFunction(TypeList<TransformType_t<OutputTypes>...>{}, TypeList<TransformType_t<InputTypes>...>{}),
			.mExecuteFunction = CreateExecuteNodeFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext, NodeExecutionContextType, NodeStateDataType, Callable>(aOutputList,  TypeList<TransformType_t<InputTypes>...>{}),
			.mTraits = traits,
			.mEventID = aCreationData.mEventID,
			.mOwnerDataTypeID = aCreationData.mOwnerDataTypeID,
			.mInputPinTypeIDs = inputPinTypeIDs,
			.mOutputPinTypeIDs = outputPinTypeIDs,
			.mFunctionMemoryID = functionMemoryID,
			.mNodeStateDataTypeID = nodeStateDataTypeID,
			.mName = aCreationData.mName,
			.mOperatorTrait = aCreationData.mOperatorTrait,
		};
	}



	// For function with 1 return value
	template<eNodeTrait Traits = eNodeTrait::None, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits>(
					[aFunction](Flow, InputTypes... someInputs) -> Flow
					{
						aFunction(std::forward<InputTypes>(someInputs)...);
						return Flow(true);
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits>(
					[aFunction](Flow, InputTypes... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(std::forward<InputTypes>(someInputs)...);
						return { Flow(true), output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits>(
					aFunction,
					TypeList<>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits>(
					aFunction,
					TypeList<OutputType>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
		}
	}

	// For functions with 1 return value && takes in an execution context
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputTypeVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputTypeVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aContext, std::forward<InputTypes>(someInputs)...);
						return { Flow(true) };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aContext, std::forward<InputTypes>(someInputs)...);
						return { Flow(true), output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputTypeVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(
					aFunction,
					TypeList<>(),
					TypeList<InputTypes...>(),
					aCreationData
				);

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(
					aFunction,
					TypeList<OutputType>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
		}
	}

	// For functions with 1 return value and takes in internal data
	template<eNodeTrait Traits = eNodeTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
					[aFunction](NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aInternalData, someInputs...);
						return { Flow(true) };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
					[aFunction](NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aInternalData, someInputs...);
						return { Flow(true), output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
					aFunction,
					TypeList<>(),
					TypeList<InputTypes...>(),
					aCreationData
				);

			}
			else
			{
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
					aFunction,
					TypeList<OutputType>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
		}
	}

	// For functions with 1 return value && takes in execution context && takes in internal data 
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, ExecutionContextType, NodeStateDataType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...);
						return { Flow(true) };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionContextType, NodeStateDataType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...);
						return { Flow(true), output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(
					aFunction,
					TypeList<>(),
					TypeList<InputTypes...>(),
					aCreationData
				);

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(
					aFunction,
					TypeList<OutputType>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
		}
	}

	// For functions with tuple return value
	template<eNodeTrait Traits = eNodeTrait::None, typename... OutputTypes, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits>(
				[aFunction](Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(Flow(true)), aFunction(std::forward<InputTypes>(aInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(),
				aCreationData
			);

		}
		else
		{

			return CreateNodeRecipe<Traits>(
				aFunction,
				TypeList<OutputTypes...>(),
				TypeList<InputTypes...>(),
				aCreationData
			);

		}
	}

	// For functions with tuple return value && takes in execution context
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename... OutputTypes, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(
				[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(Flow(true)), aFunction(aContext, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(),
				aCreationData
			);

		}
		else
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(aFunction,
				TypeList<OutputTypes...>(),
				TypeList<InputTypes...>(),
				aCreationData
			);
		}
	}

	// For functions with tuple return value && takes in internal data
	template<eNodeTrait Traits = eNodeTrait::None, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
				[aFunction](NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(Flow(true)), aFunction(aInternalData, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(),
				aCreationData
			);

		}
		else
		{
			return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(
				aFunction,
				TypeList<OutputTypes...>(),
				TypeList<InputTypes...>(),
				aCreationData
			);
		}
	}

	// For functions with tuple return value && takes in execution context && takes in NodeState
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(
				[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(Flow(true)), aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(),
				aCreationData
			);
		}
		else
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(aFunction,
				TypeList<OutputTypes...>(),
				TypeList<InputTypes...>(),
				aCreationData
			);
		}
	}


	// For functions with 1 return value and takes in InternalExecutionContext
	template<eNodeTrait Traits = eNodeTrait::None, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, InternalExecutionContextPtr, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](InternalExecutionContextPtr aContext, Flow, InputTypes... aInputs) -> Flow
					{
						aFunction(aContext, std::forward<InputTypes>(aInputs)...);
						return Flow(true);
					},
					TypeList<Flow>(), TypeList<Flow, InputTypes...>(), aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](InternalExecutionContextPtr aContext, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
					{
						return { Flow(true), aFunction(aContext, std::forward<InputTypes>(aInputs)...) };
					},
					TypeList<Flow, OutputType>(), TypeList<Flow, InputTypes...>(), aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](InternalExecutionContextPtr aContext, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					},
					TypeList<>(), TypeList<InputTypes...>(), aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](InternalExecutionContextPtr aContext, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					},
					TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData
				);
			}
		}
	}


	// For functions with 1 return value and takes in InternalExecutionContext and NodeState
	template<eNodeTrait Traits = eNodeTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, InternalExecutionContextPtr, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = std::same_as<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, Wildcard, NodeStateDataType>(
					[aFunction](InternalExecutionContextPtr aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> Flow
					{
						aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
						return Flow(true);
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, Wildcard, NodeStateDataType>(
					[aFunction](InternalExecutionContextPtr aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
					{
						return { Flow(true), aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...) };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(),
					aCreationData
				);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, Wildcard, NodeStateDataType>(
					[aFunction](InternalExecutionContextPtr aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
					},
					TypeList<>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, Wildcard, NodeStateDataType>(
					[aFunction](InternalExecutionContextPtr aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
					},
					TypeList<OutputType>(),
					TypeList<InputTypes...>(),
					aCreationData
				);
			}
		}
	}

	// For functions with tuple return value and takes in InternalExecutionContext
	template<eNodeTrait Traits = eNodeTrait::None, typename... OutputTypes, typename... InputTypes>
	constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, InternalExecutionContextPtr, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
				[aFunction](InternalExecutionContextPtr aContext, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return { Flow(true), aFunction(aContext, std::forward<InputTypes>(aInputs)...) };
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(),
				aCreationData
			);
		}
		else
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
				[aFunction](InternalExecutionContextPtr aContext, InputTypes... aInputs) -> std::tuple<OutputTypes...>
				{
					return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
				},
				TypeList<OutputTypes...>(),
				TypeList<InputTypes...>(),
				aCreationData
			);
		}
	}

	// non-const member functions with non-void return
	template<typename ClassType, typename OutputType, typename... InputTypes>
	NodeRecipe FilterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		auto callable = [aFunction](ClassType* aClassType, InputTypes&&... aInputTypes) -> OutputType
			{
				return (aClassType->*aFunction)(std::forward<InputTypes>(aInputTypes)...);
			};
		return CreateNodeRecipe(
			callable,
			TypeList<OutputType>(),
			TypeList<ClassType*, InputTypes...>(),
			aCreationData
		);
	}

	// void returning member functions - generates flow
	template<typename ClassType, typename OutputType, typename... InputTypes> requires std::same_as<OutputType, void>
	NodeRecipe FilterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		auto callable = [aFunction](Flow, ClassType* aClassType, InputTypes&&... aInputTypes) -> Flow
			{
				(aClassType->*aFunction)(std::forward<InputTypes>(aInputTypes)...);
				return Flow(true);
			};
		return CreateNodeRecipe<eNodeTrait::HasImplicitFlow>(
			callable,
			TypeList<Flow>(),
			TypeList<Flow, ClassType*, InputTypes...>(),
			aCreationData
		);
	}

	// const member functions
	template<typename ClassType, typename OutputType, typename... InputTypes>
	NodeRecipe FilterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		auto callable = [aFunction](ClassType* aClassType, InputTypes&&... aInputTypes) -> OutputType
			{
				return (aClassType->*aFunction)(std::forward<InputTypes>(aInputTypes)...);
			};
		return CreateNodeRecipe(
			callable,
			TypeList<OutputType>(),
			TypeList<ClassType*, InputTypes...>(),
			aCreationData
		);
	}
}