#pragma once
#include "../ScriptDefines.h"
#include "../DataStructures/FunctionTuple.h"
#include "NodeState.h"
#include "../Contexts/InternalExecutionContext.h"
#include "../Variable/VariableManager.h"
#include "NodeTypeManager.h"
#include "NodeExecutor.h"
#include "../Pin/PinManager.h"
#include "Node.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Pin/PinTypeManager.h"
#include "../Memory/ScriptMemoryPool.h"
#include "../DataType/DataTypeManager.h"
#include "../ScriptInternalModifier.h"
#include "../SystemTypes/ScriptWildcard.h"
#include "../SystemTypes/ScriptFlow.h"
#include "../Utilities/MetaScript.h"
#include "../Global/ScriptGlobal.h"
#include "../Instance/ScriptInstance.h"

namespace SCR
{

	template<typename... Inputs>
	using MemoryTuple = AccessorTuple<ReturnList<Inputs...>, ConstantList<MemoryPool*>, VariedList<MemoryPoolID>>;

	template<typename... Inputs>
	using MemoryTuple2 = AccessorTuple2<ReturnList<Inputs...>, ConstantList<MemoryPool*>, VariedList<MemoryPoolID>>;

	PinSetFunction CreatePinSetFunction();

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, const NodeGraph& aDestinationNodeGraph, const NodeGraph& aSourceNodeGraph, const size_t aStartIndex);

	template<CleanType T, eFlowType FlowType>
	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{
#ifdef FLY_DEBUG
				assert(aPinSetData.dataTypeID == typeid(T).hash_code());
#endif

				const T& value = *reinterpret_cast<const T*>(aPinSetData.value);

				const Pin& pin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, aPinSetData.id);

				T& memoryValue = *reinterpret_cast<T*>(pin.dataPtr);
				memoryValue = value;

				if constexpr (IsSameType<T, Flow>)
				{
					if (value)
					{
						if constexpr (FlowType == eFlowType::Output)
						{

							for (PinID connectedInputPinID : pin.connectedPinIDs)
							{
								const Pin& connectedInputPin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, connectedInputPinID);
								aContext.executionQueue->Push({ NodeRef{connectedInputPin.nodeID, aContext.nodeData.nodeRef.nodeGraph }, eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	template<size_t Index, size_t Size, eFlowType FlowType, typename MemoryTupleType, typename Type, typename... Types>
	void CreatePinTypesInternal(std::vector<PinTypeID>& aPinTypeIDArray, MemoryTupleType& aMemoryTuple, const std::array<std::string, Size>& aPinNames)
	{
		if constexpr (Index < Size)
		{
			using CT = CleanType_V<Type>;

			aPinTypeIDArray[Index] = PinTypeManager::Create<FlowType, CT>(aPinNames[Index], CreatePinSetFunction<CT, FlowType>());

			aMemoryTuple.At<Index>() = [](MemoryPool* aMemoryPool, MemoryPoolID anID) -> CT&
				{
					return aMemoryPool->At<CT>(anID);
				};

			if constexpr (Index + 1 < Size)
			{
				CreatePinTypesInternal<Index + 1, Size, FlowType, MemoryTupleType, Types...>(aPinTypeIDArray, aMemoryTuple, aPinNames);

			}
		}
	}

	template< eFlowType FlowType, typename MemoryTupleType, typename... Types>
	std::vector<PinTypeID> CreatePinTypes(MemoryTupleType& aMemoryTuple, const std::array<std::string, sizeof...(Types)>& aPinNames = std::array<std::string, sizeof...(Types)>())
	{
		constexpr size_t Size = sizeof...(Types);
		std::vector<PinTypeID> pinTypeIDArray(Size);

		if constexpr (!EmptyParameterPack<Types...>)
		{
			CreatePinTypesInternal<0, Size, FlowType, MemoryTupleType, Types...>(pinTypeIDArray, aMemoryTuple, aPinNames);
		}
		return pinTypeIDArray;
	}

	template<size_t Index, size_t Size, eFlowType FlowType, typename Type, typename... Types>
	void CreatePinTypesInternal(std::vector<PinTypeID>& aPinTypeIDArray, const std::array<std::string, Size>& aPinNames)
	{
		if constexpr (Index < Size)
		{
			using CT = CleanType_V<Type>;

			aPinTypeIDArray[Index] = PinTypeManager::Create<FlowType, CT>(aPinNames[Index], CreatePinSetFunction<CT, FlowType>());


			if constexpr (Index + 1 < Size)
			{
				CreatePinTypesInternal<Index + 1, Size, FlowType, Types...>(aPinTypeIDArray, aPinNames);

			}
		}
	}

	template<eFlowType FlowType, typename... Types>
	std::vector<PinTypeID> CreatePinTypes(const std::array<std::string, sizeof...(Types)>& aPinNames = std::array<std::string, sizeof...(Types)>())
	{
		constexpr size_t Size = sizeof...(Types);
		std::vector<PinTypeID> pinTypeIDArray(Size);

		if constexpr (!EmptyParameterPack<Types...>)
		{
			CreatePinTypesInternal<0, Size, FlowType, Types...>(pinTypeIDArray, aPinNames);
		}
		return pinTypeIDArray;
	}

	// Creates an input for a node instance
	template<typename InputType>
	PinID CreateInputPin(const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t anIndex, NodeGraph& aNodeGraph)
	{
		static_assert(!(std::is_reference_v<InputType> && !std::is_const_v<std::remove_reference_t<InputType>>), "Non const references are not supported");

		using CleanType = CleanType_V<InputType>;

		MemoryArena<NodeBufferCapacity>& memoryArena = ScriptProxy::GetNodeGraphMemoryArena(aNodeGraph);
		void* dataPtr = &memoryArena.Allocate<CleanType>();

		const PinTypeID pinTypeID = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID).nodeRecipe.inputPinTypeIDs[anIndex];

		return InternalModifier::CreatePin(aNodeGraph, aNodeID, pinTypeID, dataPtr);

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

	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& anInternalExecutionContext, size_t aStartIndex = 0);

	template<typename OutputType>
	PinID CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, NodeGraph& aNodeGraph)
	{
		static_assert(!std::is_reference_v<OutputType>, "Return type can't be a reference");
		static_assert(!std::is_same_v<void, OutputType>, "Return type can't be void");

		//MemoryPoolID memoryPoolID = ScriptProxy::GetGraphMemoryPool(aNodeGraph).Allocate<OutputType>();
		void* dataPtr = &ScriptProxy::GetNodeGraphMemoryArena(aNodeGraph).Allocate<OutputType>();
		return InternalModifier::CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
	}

	template<size_t Index, size_t OutputSize, typename OutputType, typename... OutputTypes>
	void CreateOutputPinsInternal(const NodeID aNodeID, const NodeTypeID aNodeTypeID, std::array<PinID, OutputSize>& aPinIDs, NodeGraph& aNodeGraph)
	{
		if constexpr (Index < OutputSize)
		{

			const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID);
			const PinTypeID pinTypeID = nodeType.nodeRecipe.outputPinTypeIDs.at(Index);

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

	template<size_t Index = 0, typename... OutputTypes>
	void SetOutputValues(const std::tuple<OutputTypes...>& aOutputValues, const std::vector<PinID>& aOutputPinIDs, InternalExecutionContext& aContext)
	{
		if constexpr (Index < sizeof...(OutputTypes))
		{
			const PinID outputPinID = aOutputPinIDs[Index];
			const Pin& pin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, outputPinID);
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);
			assert(pinType.flowType == eFlowType::Output);

			const void* value = &std::get<Index>(aOutputValues);


			pinType.setFunction(PinSetData{ outputPinID, value,

#ifdef FLY_DEBUG
				typeid(decltype(std::get<Index>(aOutputValues))).hash_code()
#endif
				}, aContext);

			SetOutputValues<Index + 1>(aOutputValues, aOutputPinIDs, aContext);
		}
	}

	// Function definitions.
	template<typename TupleType, size_t Index, typename Arg, typename... Args>
	void CreateInputPackInternal(TupleType& aTuple, const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		if constexpr (Index < std::tuple_size_v<TupleType>)
		{
			const Pin& pin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, aPinIDs[Index]);
			std::get<Index>(aTuple) = ReferenceWrapper<Arg>(*reinterpret_cast<Arg*>(pin.dataPtr));

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

	template<typename...>
	struct TypeList {};

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

				std::vector<PinID> addedInputPinIDs = InternalModifier::CreateInputPins(aNodeGraph, aNodeID, aNodeTypeID, preExistingInputPinIDs.size());

				const std::vector<PinID> totalInputPinIDs = AppendContainers<std::vector<PinID>>(std::move(preExistingInputPinIDs), std::move(addedInputPinIDs));

				// Output pins
				std::array<PinID, PinOutputSize> preExistingOutputPinIDs = CreateOutputPins<OutputTypes...>(aNodeID, aNodeTypeID, aNodeGraph);

				std::vector<PinID> addedOutputPinIDs = InternalModifier::CreateOutputPins(aNodeGraph, aNodeID, aNodeTypeID, preExistingOutputPinIDs.size());

				const std::vector<PinID> totalOutputPinIDs = AppendContainers<std::vector<PinID>>(std::move(preExistingOutputPinIDs), std::move(addedOutputPinIDs));

				return Node(aNodeTypeID, totalInputPinIDs, totalOutputPinIDs);
			};

	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	std::tuple<OutputTypes...> CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		const Node& node = ScriptProxy::GetNode(*aContext.nodeData.nodeRef.nodeGraph, aContext.nodeData.nodeRef.nodeID);
		const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(node.typeID);

		MemoryPool& foundationMemoryPool = ScriptProxy::GetGlobalMemoryPool();

		MemoryPoolID functionMemoryID = nodeType.nodeRecipe.functionMemoryID;
		Callable& callable = foundationMemoryPool.At<Callable>(functionMemoryID);

		std::tuple<ReferenceWrapper<InputTypes>...> inputTuple = CreateInputPack<InputTypes...>(node.inputPins, aContext);
		inputTuple;

		if constexpr (TakesExecutionContext && TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.nodeGraphInstance->myNodeManagerInstance.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeRef.nodeID)
			};

			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext, nodeState), inputTuple));
		}
		else if constexpr (TakesExecutionContext)
		{
			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext), inputTuple));
		}
		else if constexpr (TakesNodeState && TakesInternalExecutionContext)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.nodeGraphInstance->myNodeManagerInstance.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeRef.nodeID)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(&aContext, nodeState), inputTuple));

		}
		else if constexpr (TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.nodeGraphInstance->myNodeManagerInstance.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeRef.nodeID)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(nodeState), inputTuple));
		}
		else if constexpr (TakesInternalExecutionContext)
		{
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(&aContext), inputTuple));
		}
		else
		{
			return std::apply(callable, inputTuple);
		}
	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	ExecuteNodeSignature CreateExecuteNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		constexpr bool HasInputs = !EmptyParameterPack<InputTypes...>;

		return [](const NodeExecutionData& aNodeExecutionData, InternalExecutionContext& aContext) -> void
			{

				const Node& node = ScriptProxy::GetNode(*aNodeExecutionData.nodeRef.nodeGraph, aNodeExecutionData.nodeRef.nodeID);

				// Evaluate input values
				if constexpr (HasInputs)
				{
					EvaluateInputValues(node.inputPins, aContext);
				}

				// Set current node data before calling function
				aContext.nodeData = aNodeExecutionData;


				ExecutionQueue executionQueue;
				aContext.executionQueue = &executionQueue;

				// Call function and retrieve output values
				std::tuple<OutputTypes...> outputValues = CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext,
					NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});

				// Set output of function
				SetOutputValues(std::forward<std::tuple<OutputTypes...>>(std::move(outputValues)), node.outputPins, aContext);

				aContext.executionQueue = nullptr;
				executionQueue.Execute();
			};
	}

	struct NodeCreationData
	{
		EventID eventID = InvalidID<EventID>();
		eNodeOperatorTrait operatorTrait = eNodeOperatorTrait::None;
		DataTypeID ownerDataTypeID = GlobalDataTypeID;
		NodeTypeDesc desc;
	};

	template<eNodeTrait Traits = eNodeTrait::None, typename NodeExecutionContextType = Wildcard, typename NodeStateDataType = Wildcard, typename Callable, typename... OutputTypes, typename... InputTypes>
	NodeRecipe CreateNodeRecipe(Callable aCallable, TypeList<OutputTypes...> aOutputList, TypeList<InputTypes...>, const NodeCreationData& aNodeCreationData)
	{
		static_assert(sizeof...(OutputTypes) > 0, "A node must always have an output pin, have you considered registering it as a flow node type?");

		constexpr bool TakesExecutionContext = HasFlag(Traits, eNodeTrait::TakesExecutionContext);
		constexpr bool TakesNodeState = !IsSameType<Wildcard, NodeStateDataType>;
		constexpr bool TakesInternalExecutionContext = HasFlag(Traits, eNodeTrait::TakesInternalExecutionContext);
		constexpr bool HasFlow = TypeExists<Flow, OutputTypes...> || TypeExists<Flow, InputTypes...>;

		DataTypeID nodeStateDataTypeID = InvalidID<DataTypeID>();
		if constexpr (TakesNodeState)
		{
			Global::GetDataTypeManager().Register<NodeStateDataType>(typeid(NodeStateDataType).name());
			nodeStateDataTypeID = typeid(NodeStateDataType).hash_code();
		}

		eNodeTrait traits = Traits;
		if constexpr (HasFlow)
		{
			traits |= eNodeTrait::HasFlow;
		}

		std::vector<PinTypeID> inputPinTypeIDs = CreatePinTypes<eFlowType::Input, std::remove_cvref_t<InputTypes>...>();
		std::vector<PinTypeID> outputPinTypeIDs = CreatePinTypes<eFlowType::Output, OutputTypes...>();

		MemoryPool& foundationMemoryPool = ScriptProxy::GetGlobalMemoryPool();
		MemoryPoolID functionMemoryID = foundationMemoryPool.Allocate<Callable>(aCallable);

		return NodeRecipe
		{
			.createFunction = CreateCreateNodeFunction(aOutputList, TypeList<std::remove_cvref_t<InputTypes>...>{}),
			.executeFunction = CreateExecuteNodeFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext, NodeExecutionContextType, NodeStateDataType, Callable>(aOutputList,  TypeList<std::remove_cvref_t<InputTypes>...>{}),
			.traits = traits,
			.eventID = aNodeCreationData.eventID,
			.operatorTrait = aNodeCreationData.operatorTrait,
			.ownerDataTypeID = aNodeCreationData.ownerDataTypeID,
			.inputPinTypeIDs = inputPinTypeIDs,
			.outputPinTypeIDs = outputPinTypeIDs,
			.functionMemoryID = functionMemoryID,
			.nodeStateDataTypeID = nodeStateDataTypeID
		};
	}



	// For function with 1 return value
	template<eNodeTrait Traits = eNodeTrait::None, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
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
					aCreationData);

			}
			else
			{
				return CreateNodeRecipe<Traits>(
					[aFunction](Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(std::forward<InputTypes>(someInputs)...);
						return { Flow(true), output};
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
				return CreateNodeRecipe<Traits>(aFunction, TypeList<>(), TypeList<InputTypes...>(), aCreationData);
			}
			else
			{
				return CreateNodeRecipe<Traits>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData);
			}
		}
	}

	// For functions with 1 return value && takes in an execution context
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputTypeVoid = IsSameType<OutputType, void>;
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
						return { Flow(true), output};
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
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(aFunction, TypeList<>(), TypeList<InputTypes...>(), aCreationData);

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData);
			}
		}
	}

	// For functions with 1 return value and takes in internal data
	template<eNodeTrait Traits = eNodeTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
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
						return { Flow(true), output};
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
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(aFunction, TypeList<>(), TypeList<InputTypes...>(), aCreationData);

			}
			else
			{
				return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData);
			}
		}
	}

	// For functions with 1 return value && takes in execution context && takes in internal data 
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
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
					TypeList<Flow, InputTypes...>(), aCreationData);
			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionContextType, NodeStateDataType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...);
						return { Flow(true), output};
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>(), aCreationData);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(aFunction, TypeList<>(), TypeList<InputTypes...>(), aCreationData);

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionContextType, NodeStateDataType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData);
			}
		}
	}

	// For functions with tuple return value
	template<eNodeTrait Traits = eNodeTrait::None, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits>(
				[aFunction](Flow, InputTypes... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(Flow(true)), aFunction(std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>(), aCreationData);

		}
		else
		{

			return CreateNodeRecipe<Traits>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>(), aCreationData);

		}
	}

	// For functions with tuple return value && takes in execution context
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
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
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
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
			return CreateNodeRecipe<Traits, Wildcard, NodeStateDataType>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>(), aCreationData);
		}
	}

	// For functions with tuple return value && takes in execution context && takes in NodeState
	template<eNodeTrait Traits = eNodeTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
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
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, const InternalExecutionContext*, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> Flow
					{
						aFunction(aContext, std::forward<InputTypes>(aInputs)...);
						return Flow(true);
					}, TypeList<Flow>(), TypeList<Flow, InputTypes...>(), aCreationData
						);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
					{
						return { Flow(true), aFunction(aContext, std::forward<InputTypes>(aInputs)...) };
					}, TypeList<Flow, OutputType>(), TypeList<Flow, InputTypes...>(), aCreationData
						);
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					}, TypeList<>(), TypeList<InputTypes...>(), aCreationData
						);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext>(
					[aFunction](const InternalExecutionContext* aContext, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					}, TypeList<OutputType>(), TypeList<InputTypes...>(), aCreationData
						);
			}
		}
	}


	// For functions with 1 return value and takes in InternalExecutionContext and NodeState
	template<eNodeTrait Traits = eNodeTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, const InternalExecutionContext*, NodeState<NodeStateDataType>, InputTypes...> aFunction, const NodeCreationData& aCreationData)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, Wildcard, NodeStateDataType>(
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> Flow
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
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
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
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
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
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
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

	template<typename ClassType, typename OutputType, typename... InputTypes>
	NodeRecipe FilterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFuncPtr)
	{
		auto callable = [aFuncPtr](ClassType* aClassType, InputTypes&&... aInputTypes) -> OutputType
			{
				return (aClassType->*aFuncPtr)(std::forward<InputTypes>(aInputTypes)...);
			};
		return CreateNodeRecipe(callable, TypeList<OutputType>(), TypeList<ClassType*, InputTypes...>(), NodeCreationData{ .ownerDataTypeID = GetDataTypeID<ClassType>() });
	}

	template<typename ClassType, typename OutputType, typename... InputTypes> requires IsSameType<OutputType, void>
	NodeRecipe FilterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFuncPtr)
	{
		auto callable = [aFuncPtr](Flow, ClassType* aClassType, InputTypes&&... aInputTypes) -> Flow
			{
				(aClassType->*aFuncPtr)(std::forward<InputTypes>(aInputTypes)...);
				return Flow(true);
			};
		return CreateNodeRecipe<eNodeTrait::HasImplicitFlow>(callable, TypeList<Flow>(), TypeList<Flow, ClassType*, InputTypes...>(), NodeCreationData{ .ownerDataTypeID = GetDataTypeID<ClassType>() });
	}

	template<typename ClassType, typename OutputType, typename... InputTypes>
	NodeRecipe FilterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFuncPtr)
	{
		auto callable = [aFuncPtr](ClassType* aClassType, InputTypes&&... aInputTypes) -> OutputType
			{
				return (aClassType->*aFuncPtr)(std::forward<InputTypes>(aInputTypes)...);
			};
		return CreateNodeRecipe(callable, TypeList<OutputType>(), TypeList<ClassType*, InputTypes...>(), NodeCreationData{ .ownerDataTypeID = GetDataTypeID<ClassType>() });
	}
}