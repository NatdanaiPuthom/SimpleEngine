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

namespace SCR
{

	template<typename... Inputs>
	using MemoryTuple = AccessorTuple<ReturnList<Inputs...>, ConstantList<MemoryPool*>, VariedList<MemoryPoolID>>;

	template<typename... Inputs>
	using MemoryTuple2 = AccessorTuple2<ReturnList<Inputs...>, ConstantList<MemoryPool*>, VariedList<MemoryPoolID>>;

	PinSetFunction CreatePinSetFunction();

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, const size_t aStartIndex);

	template<CleanType T, ePinFlowType FlowType>
	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{
#ifdef FLY_DEBUG
				assert(aPinSetData.dataTypeID == typeid(T).hash_code());
#endif

				const T& value = *reinterpret_cast<const T*>(aPinSetData.value);

				const Pin& pin = ScriptProxy::GetPin(aContext.script, aPinSetData.id);

				T& memoryValue = ScriptProxy::GetScriptMemoryPool(aContext.script).At<T>(pin.memoryID);
				memoryValue = value;

				if constexpr (IsSameType<T, Flow>)
				{
					if (value)
					{
						if constexpr (FlowType == ePinFlowType::Output)
						{
							NodeExecutor& executor = ScriptProxy::GetNodeExecutor(aContext.script);

							for (PinID connectedInputPinID : pin.connectedPinIDs)
							{
								const Pin& connectedInputPin = ScriptProxy::GetPin(aContext.script, connectedInputPinID);
								executor.Push({ connectedInputPin.nodeID, eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	template<size_t Index, size_t Size, ePinFlowType FlowType, typename MemoryTupleType, typename Type, typename... Types>
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

	template< ePinFlowType FlowType, typename MemoryTupleType, typename... Types>
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

	template<size_t Index, size_t Size, ePinFlowType FlowType, typename Type, typename... Types>
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

	template<ePinFlowType FlowType, typename... Types>
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
	PinID CreateInputPin(const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t anIndex, ScriptInternalModifier& aModifier)
	{
		static_assert(!(std::is_reference_v<InputType> && !std::is_const_v<std::remove_reference_t<InputType>>), "Non const references are not supported");

		using CT = CleanType_V<InputType>;

		MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aModifier.myScript);

		MemoryPoolID memoryID = memoryPool.Allocate<CT>();

		const PinTypeID pinTypeID = NodeTypeManager::GetNodeType(aNodeTypeID).nodeRecipe.inputPinTypeIDs[anIndex];

		return aModifier.CreateInputPin(aNodeID, pinTypeID, memoryID);

	}
	template<size_t Index, size_t InputSize, typename InputType, typename... InputTypes>
	void CreateInputPinsInternal(const NodeID aNodeID, const NodeTypeID aNodeTypeID, std::array<PinID, InputSize>& aPinIDs, ScriptInternalModifier& aModifier)
	{
		aPinIDs[Index] = CreateInputPin<InputType>(aNodeID, aNodeTypeID, Index, aModifier);
		if constexpr (!EmptyParameterPack<InputTypes...>)
		{
			CreateInputPinsInternal<Index + 1, InputSize, InputTypes...>(aNodeID, aNodeTypeID, aPinIDs, aModifier);
		}
	}

	template<typename... Inputs>
	std::array<PinID, sizeof...(Inputs)> CreateInputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier)
	{
		constexpr size_t InputSize = sizeof...(Inputs);
		std::array<PinID, InputSize> pinIDs{};
		if constexpr (!EmptyParameterPack<Inputs...>)
		{
			CreateInputPinsInternal<0, InputSize, Inputs...>(aNodeID, aNodeTypeID, pinIDs, aModifier);
		}
		return pinIDs;
	}

	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, InternalExecutionContext& anInternalExecutionContext, size_t aStartIndex = 0);

	template<typename OutputType>
	PinID CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, ScriptInternalModifier& aModifier)
	{
		static_assert(!std::is_reference_v<OutputType>, "Return type can't be a reference");
		static_assert(!std::is_same_v<void, OutputType>, "Return type can't be void");

		MemoryPoolID memoryPoolID = ScriptProxy::GetScriptMemoryPool(aModifier.myScript).Allocate<OutputType>();

		return aModifier.CreateOutputPin(aNodeID, aPinTypeID, memoryPoolID);
	}

	template<size_t Index, size_t OutputSize, typename OutputType, typename... OutputTypes>
	void CreateOutputPinsInternal(const NodeID aNodeID, const NodeTypeID aNodeTypeID, std::array<PinID, OutputSize>& aPinIDs, ScriptInternalModifier& aModifier)
	{
		if constexpr (Index < OutputSize)
		{

			const NodeType& nodeType = NodeTypeManager::GetNodeType(aNodeTypeID);
			const PinTypeID pinTypeID = nodeType.nodeRecipe.outputPinTypeIDs.at(Index);

			aPinIDs[Index] = CreateOutputPin<OutputType>(aNodeID, pinTypeID, aModifier);

			if constexpr (Index + 1 < OutputSize)
			{
				CreateOutputPinsInternal<Index + 1, OutputSize, OutputTypes...>(aNodeID, aNodeTypeID, aPinIDs, aModifier);
			}
		}
	}

	template<typename... OutputTypes>
	std::array<PinID, sizeof...(OutputTypes)> CreateOutputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier)
	{
		std::array<PinID, sizeof...(OutputTypes)> pinIDs{};
		if constexpr (!EmptyParameterPack<OutputTypes...>)
		{
			CreateOutputPinsInternal<0, sizeof...(OutputTypes), OutputTypes...>(aNodeID, aNodeTypeID, pinIDs, aModifier);
		}
		return pinIDs;
	}

	template<size_t Index = 0, typename... OutputTypes>
	void SetOutputValues(const std::tuple<OutputTypes...>& aOutputValues, const std::vector<PinID>& aOutputPinIDs, InternalExecutionContext& aContext)
	{
		if constexpr (Index < sizeof...(OutputTypes))
		{
			const PinID outputPinID = aOutputPinIDs[Index];
			const Pin& pin = ScriptProxy::GetPin(aContext.script, outputPinID);
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);
			assert(pinType.flowType == ePinFlowType::Output);

			const void* value = &std::get<Index>(aOutputValues);


			pinType.setFunction(PinSetData{ outputPinID, value,

#ifdef FLY_DEBUG
				typeid(decltype(std::get<Index>(aOutputValues))).hash_code()
#endif
				}, aContext);

			SetOutputValues<Index + 1>(aOutputValues, aOutputPinIDs, aContext);
		}
	}

	template<size_t Size>
	std::array<std::tuple<MemoryPoolID>, Size> ToArray(const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		std::array<std::tuple<MemoryPoolID>, Size> idArray;
		for (size_t i = 0; i < Size; ++i)
		{
			const Pin& pin = ScriptProxy::GetPin(aContext.script, aPinIDs[i]);
			idArray[i] = std::tuple<MemoryPoolID>{ pin.memoryID };
		}
		return idArray;
	}

	template<size_t Size>
	std::array<MemoryPoolID, Size> ToArrayNew(const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		std::array<MemoryPoolID, Size> idArray{};
		for (size_t i = 0; i < Size; ++i)
		{
			const Pin& pin = ScriptProxy::GetPin(aContext.script, aPinIDs[i]);
			idArray[i] = pin.memoryID;
		}
		return idArray;
	}

	template<typename Return, typename A, typename B>
	Return AppendContainers(const A& a, const B& b)
	{
		Return r{};
		r.insert(r.begin(), a.begin(), a.end());
		r.insert(r.end(), b.begin(), b.end());
		return r;
	}

	template<typename...>
	struct TypeList {};

	template<bool TakesNodeState, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	CreateNodeSignature CreateCreateNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{
		return [](const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier) -> Node
			{

				constexpr size_t PinInputSize = sizeof...(InputTypes);
				constexpr size_t PinOutputSize = sizeof...(OutputTypes);

				// Create Node Function

				if constexpr (TakesNodeState)
				{
					aModifier.AddNodeState<NodeStateDataType>(aNodeID);
				}

				// Input pins
				std::array<PinID, PinInputSize> preExistingInputPinIDs = CreateInputPins<InputTypes...>(aNodeID, aNodeTypeID, aModifier);

				std::vector<PinID> addedInputPinIDs = aModifier.CreateInputPins(aNodeID, aNodeTypeID, preExistingInputPinIDs.size());

				std::vector<PinID> totalInputPinIDs = AppendContainers<std::vector<PinID>>(preExistingInputPinIDs, addedInputPinIDs);

				// Output pins
				std::array<PinID, PinOutputSize> preExistingOutputPinIDs = CreateOutputPins<OutputTypes...>(aNodeID, aNodeTypeID, aModifier);

				std::vector<PinID> addedOutputPinIDs = aModifier.CreateOutputPins(aNodeID, aNodeTypeID, preExistingOutputPinIDs.size());

				std::vector<PinID> totalOutputPinIDs = AppendContainers<std::vector<PinID>>(preExistingOutputPinIDs, addedOutputPinIDs);

				return Node(aNodeTypeID, totalInputPinIDs, totalOutputPinIDs);
			};

	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	std::tuple<OutputTypes...> CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{


		constexpr size_t PinInputSize = sizeof...(InputTypes);

		using MemoryTupleType = MemoryTupleNew<InputTypes...>;

		// Node internal functionality

		const Node& node = ScriptProxy::GetNode(aContext.script, aContext.nodeData.nodeID);
		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		MemoryPool& foundationMemoryPool = ScriptProxy::GetGlobalMemoryPool();

		MemoryPoolID memoryTupleMemoryID = nodeType.nodeRecipe.tupleMemoryID;
		MemoryTupleType& memoryTuple = foundationMemoryPool.At<MemoryTupleType>(memoryTupleMemoryID);

		MemoryPoolID functionMemoryID = nodeType.nodeRecipe.functionMemoryID;
		Callable& callable = foundationMemoryPool.At<Callable>(functionMemoryID);


		MemoryPool* memoryPool = &ScriptProxy::GetScriptMemoryPool(aContext.script);

		if constexpr (TakesExecutionContext && TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.modifier.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeID)
			};

			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
			};
			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext), executionContext, nodeState);
		}
		else if constexpr (TakesExecutionContext)
		{
			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
			};
			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext), executionContext);
		}
		else if constexpr (TakesNodeState && TakesInternalExecutionContext)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.modifier.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeID)
			};

			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext), &aContext, nodeState);

		}
		else if constexpr (TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.modifier.GetNodeState<NodeStateDataType>(aContext.nodeData.nodeID)
			};

			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext), nodeState);
		}
		else if constexpr (TakesInternalExecutionContext)
		{
			//return std::tuple<OutputTypes...>{};
			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext), &aContext);
		}
		else
		{
			//return memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext));
			return memoryTuple.Call(callable, memoryPool, ToArrayNew<PinInputSize>(node.inputPins, aContext));
			//return std::tuple<OutputTypes...>{};
		}
	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	ExecuteNodeSignature CreateExecuteNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		constexpr bool HasInputs = !EmptyParameterPack<InputTypes...>;

		return [](const NodeExecutionData& aNodeExecutionData, InternalExecutionContext& aContext) -> void
			{

				const Node& node = ScriptProxy::GetNode(aContext.script, aNodeExecutionData.nodeID);

				// Evaluate input values
				if constexpr (HasInputs)
				{
					EvaluateInputValues(node.inputPins, aContext);
				}

				// Set current node data before calling function
				aContext.nodeData = aNodeExecutionData;

				//// Call function and retrieve output values
				//std::tuple<OutputTypes...> outputValues;

				//if constexpr (TakesExecutionContext && TakesNodeState)
				//{
				//	NodeState<NodeStateDataType> nodeState
				//	{
				//		aContext.modifier.GetNodeState<NodeStateDataType>(aNodeExecutionData.currentNodeID)
				//	};

				//	NodeExecutionContext<NodeExecutionContextType> executionContext
				//	{
				//		*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
				//	};
				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext), executionContext, nodeState);
				//}
				//else if constexpr (TakesExecutionContext)
				//{
				//	NodeExecutionContext<NodeExecutionContextType> executionContext
				//	{
				//		*reinterpret_cast<const NodeExecutionContextType*>(aContext.executionContext)
				//	};
				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext), executionContext);
				//}
				//else if constexpr (TakesNodeState && TakesInternalExecutionContext)
				//{
				//	NodeState<NodeStateDataType> nodeState
				//	{
				//		aContext.modifier.GetNodeState<NodeStateDataType>(aNodeExecutionData.currentNodeID)
				//	};

				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext), &aContext, nodeState);

				//}
				//else if constexpr (TakesNodeState)
				//{
				//	NodeState<NodeStateDataType> nodeState
				//	{
				//		aContext.modifier.GetNodeState<NodeStateDataType>(aNodeExecutionData.currentNodeID)
				//	};

				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext), nodeState);
				//}
				//else if constexpr (TakesInternalExecutionContext)
				//{
				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext), &aContext);
				//}
				//else
				//{
				//	outputValues = memoryTuple.Call(callable, memoryPool, ToArray<PinInputSize>(node.inputPins, aContext));
				//}

				std::tuple<OutputTypes...> outputValues = CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext, 
					NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});

				// Set output of function
				SetOutputValues(std::forward<std::tuple<OutputTypes...>>(outputValues), node.outputPins, aContext);

			};
	}

	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename NodeExecutionContextType = Wildcard, typename NodeStateDataType = Wildcard, typename Callable, typename... OutputTypes, typename... InputTypes>
	NodeRecipe CreateNodeRecipe(Callable aCallable, TypeList<OutputTypes...> aOutputList, TypeList<InputTypes...>)
	{
		static_assert(sizeof...(OutputTypes) > 0, "A node must always have an output pin, have you considered registering it as a flow node type?");

		constexpr bool TakesExecutionContext = HasFlag(Traits, eNodeTrait::TakesExecutionContext);
		constexpr bool TakesNodeState = !IsSameType<Wildcard, NodeStateDataType>;
		constexpr bool TakesInternalExecutionContext = HasFlag(Traits, eNodeTrait::TakesInternalExecutionContext);
		constexpr bool HasFlow = TypeExists<Flow, OutputTypes...> || TypeExists<Flow, InputTypes...>;

		eNodeTrait traits = Traits;
		if constexpr (HasFlow)
		{
			traits |= eNodeTrait::HasFlow;
		}

		//static_assert(false);

		//using CleanInputTypes = RemoveReferences<InputTypes...>;
		using MemoryTupleType = MemoryTupleNew<std::remove_cvref_t<InputTypes>...>;
		MemoryTupleType memoryTupleTest;
		std::vector<PinTypeID> inputPinTypes = CreatePinTypes<ePinFlowType::Input, MemoryTupleType, std::remove_cvref_t<InputTypes>...>(memoryTupleTest);
		std::vector<PinTypeID> outputPinTypes = CreatePinTypes<ePinFlowType::Output, OutputTypes...>();

		MemoryPool& foundationMemoryPool = ScriptProxy::GetGlobalMemoryPool();
		MemoryPoolID functionMemoryID = foundationMemoryPool.Allocate<Callable>(aCallable);
		MemoryPoolID tupleMemoryID = foundationMemoryPool.Allocate<MemoryTupleType>(memoryTupleTest);

		return NodeRecipe
		{
			CreateCreateNodeFunction<TakesNodeState, NodeStateDataType>(aOutputList, TypeList<std::remove_cvref_t<InputTypes>...>{}),
			CreateExecuteNodeFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext, NodeExecutionContextType, NodeStateDataType, Callable>(aOutputList,  TypeList<std::remove_cvref_t<InputTypes>...>{}),
			traits,
			ExecutionTrait,
			OperatorTrait,
			inputPinTypes,
			outputPinTypes,
			tupleMemoryID,
			functionMemoryID
		};
	}



	// For function with 1 return value
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, InputTypes...> aFunction)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				auto l = [aFunction](Flow, InputTypes... someInputs) -> Flow
					{
						aFunction(std::forward<InputTypes>(someInputs)...);
						return true;
					};
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(l, TypeList<Flow>(),
					TypeList<Flow, InputTypes...>());

			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(
					[aFunction](Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(std::forward<InputTypes>(someInputs)...);
						return { aFlow, output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>());
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(aFunction, TypeList<>(), TypeList<InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>());
			}
		}
	}

	// For functions with 1 return value && takes in an execution context
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename ExecutionContextType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction)
	{
		constexpr bool IsOutputTypeVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputTypeVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aContext, std::forward<InputTypes>(someInputs)...);
						return { aFlow };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>()
				);
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aContext, std::forward<InputTypes>(someInputs)...);
						return { aFlow, output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>()
				);
			}
		}
		else
		{
			if constexpr (IsOutputTypeVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(aFunction, TypeList<>(), TypeList<InputTypes...>());

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>());
			}
		}
	}

	// For functions with 1 return value and takes in internal data
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeState<NodeStateDataType>, InputTypes...> aFunction)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aInternalData, someInputs...);
						return { aFlow };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aInternalData, someInputs...);
						return { aFlow, output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>());
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(aFunction, TypeList<>(), TypeList<InputTypes...>());

			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>());
			}
		}
	}

	// For functions with 1 return value && takes in execution context && takes in internal data 
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow>
					{
						aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...);
						return { aFlow };
					},
					TypeList<Flow>(),
					TypeList<Flow, InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(
					[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputType>
					{
						OutputType output = aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...);
						return { aFlow, output };
					},
					TypeList<Flow, OutputType>(),
					TypeList<Flow, InputTypes...>());
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(aFunction, TypeList<>(), TypeList<InputTypes...>());

			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(aFunction, TypeList<OutputType>(), TypeList<InputTypes...>());
			}
		}
	}

	// For functions with tuple return value
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, InputTypes...> aFunction)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(
				[aFunction](Flow aFlow, InputTypes... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(aFlow), aFunction(std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>());

		}
		else
		{

			return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>());

		}
	}

	// For functions with tuple return value && takes in execution context
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename ExecutionContextType, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, InputTypes...> aFunction)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(
				[aFunction](NodeExecutionContext<ExecutionContextType> aContext, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(aFlow), aFunction(aContext, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>()
			);

		}
		else
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>());
		}
	}

	// For functions with tuple return value && takes in internal data
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeState<NodeStateDataType>, InputTypes...> aFunction)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
				[aFunction](NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(aFlow), aFunction(aInternalData, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>()
			);

		}
		else
		{
			return CreateNodeRecipe<Traits, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>());
		}
	}

	// For functions with tuple return value && takes in execution context && takes in NodeState
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename ExecutionContextType, typename NodeStateDataType, typename... OutputTypes, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<std::tuple<OutputTypes...>, NodeExecutionContext<ExecutionContextType>, NodeState<NodeStateDataType>, InputTypes...> aFunction)
	{
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(
				[aFunction](NodeExecutionContext<ExecutionContextType> aContext, NodeState<NodeStateDataType> aInternalData, Flow aFlow, InputTypes&&... someInputs) -> std::tuple<Flow, OutputTypes...>
				{
					return std::tuple_cat(std::make_tuple(aFlow), aFunction(aContext, aInternalData, std::forward<InputTypes>(someInputs)...));
				},
				TypeList<Flow, OutputTypes...>(),
				TypeList<Flow, InputTypes...>()
			);
		}
		else
		{
			return CreateNodeRecipe<Traits | eNodeTrait::TakesExecutionContext, ExecutionTrait, OperatorTrait, ExecutionContextType, NodeStateDataType>(aFunction, TypeList<OutputTypes...>(), TypeList<InputTypes...>());
		}
	}


	// For functions with 1 return value and takes in InternalExecutionContext
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, const InternalExecutionContext*, InputTypes...> aFunction)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> Flow
					{
						aFunction(aContext, std::forward<InputTypes>(aInputs)...);
						return true;
					}, TypeList<Flow>(), TypeList<Flow, InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
					{
						return { true, aFunction(aContext, std::forward<InputTypes>(aInputs)...) };
					}, TypeList<Flow, OutputType>(), TypeList<Flow, InputTypes...>());
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait>(
					[aFunction](const InternalExecutionContext* aContext, Flow, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					}, TypeList<>(), TypeList<InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait>(
					[aFunction](const InternalExecutionContext* aContext, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, std::forward<InputTypes>(aInputs)...);
					}, TypeList<OutputType>(), TypeList<InputTypes...>());
			}
		}
	}


	// For functions with 1 return value and takes in InternalExecutionContext and NodeState
	template<eNodeTrait Traits = eNodeTrait::None, eNodeExecutionTrait ExecutionTrait = eNodeExecutionTrait::None, eNodeOperatorTrait OperatorTrait = eNodeOperatorTrait::None, typename NodeStateDataType, typename OutputType, typename... InputTypes>
	static constexpr NodeRecipe FilterNodeType(FuncPtr<OutputType, const InternalExecutionContext*, NodeState<NodeStateDataType>, InputTypes...> aFunction)
	{
		constexpr bool IsOutputVoid = IsSameType<OutputType, void>;
		if constexpr (HasFlag(Traits, eNodeTrait::HasImplicitFlow))
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> Flow
					{
						aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
						return true;
					}, TypeList<Flow>(), TypeList<Flow, InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, Flow, InputTypes... aInputs) -> std::tuple<Flow, OutputType>
					{
						return { true, aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...) };
					}, TypeList<Flow, OutputType>(), TypeList<Flow, InputTypes...>());
			}
		}
		else
		{
			if constexpr (IsOutputVoid)
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
					}, TypeList<>(), TypeList<InputTypes...>());
			}
			else
			{
				return CreateNodeRecipe<Traits | eNodeTrait::TakesInternalExecutionContext, ExecutionTrait, OperatorTrait, Wildcard, NodeStateDataType>(
					[aFunction](const InternalExecutionContext* aContext, NodeState<NodeStateDataType> aNodeState, InputTypes... aInputs) -> OutputType
					{
						return aFunction(aContext, aNodeState, std::forward<InputTypes>(aInputs)...);
					}, TypeList<OutputType>(), TypeList<InputTypes...>());
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
		return CreateNodeRecipe(callable, TypeList<OutputType>(), TypeList<ClassType*, InputTypes...>());
	}

	template<typename ClassType, typename OutputType, typename... InputTypes> requires IsSameType<OutputType, void>
	NodeRecipe FilterMemberNodeType(FuncPtrMember<ClassType, OutputType, InputTypes...> aFuncPtr)
	{
		auto callable = [aFuncPtr](Flow, ClassType* aClassType, InputTypes&&... aInputTypes) -> Flow
			{
				(aClassType->*aFuncPtr)(std::forward<InputTypes>(aInputTypes)...);
				return true;
			};
		return CreateNodeRecipe<eNodeTrait::HasImplicitFlow>(callable, TypeList<Flow>(), TypeList<Flow, ClassType*, InputTypes...>());
	}

	template<typename ClassType, typename OutputType, typename... InputTypes>
	NodeRecipe FilterMemberNodeType(FuncPtrMember_Const<ClassType, OutputType, InputTypes...> aFuncPtr)
	{
		auto callable = [aFuncPtr](ClassType* aClassType, InputTypes&&... aInputTypes) -> OutputType
			{
				return (aClassType->*aFuncPtr)(std::forward<InputTypes>(aInputTypes)...);
			};
		return CreateNodeRecipe(callable, TypeList<OutputType>(), TypeList<ClassType*, InputTypes...>());
	}
}