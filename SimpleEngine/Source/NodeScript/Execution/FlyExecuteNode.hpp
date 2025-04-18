#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "FlyInternalExecutionContext.hpp"
#include "FlyExecutionTypes.hpp"
#include "../SystemTypes/FlyReferenceWrapper.hpp"
#include "../Instance/FlyNodeGraphInstance.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& anInternalExecutionContext, size_t aStartIndex = 0);


	template<size_t Index = 0, typename TupleType>
	void SetOutputValues(const TupleType& aOutputValues, const std::vector<PinID>& aOutputPinIDs, InternalExecutionContext& aContext)
	{
		if constexpr (Index < std::tuple_size_v<TupleType>)
		{
			const PinID outputPinID = aOutputPinIDs[Index];
			const Pin& pin = aContext.mNodeData.mNodeRef.GetNodeGraph().GetPin(outputPinID);
			const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pin.GetTypeID());
			assert(pinType.GetIODirection() == eIODirection::Output);

			const void* dataPtr = &std::get<Index>(aOutputValues);

			SetPinValueData setPinValueData
			{
				.mNodeGraph = &aContext.mNodeData.mNodeRef.GetNodeGraph(),
				.mReadFromDataPtr = dataPtr,
				.mWriteToPinID = outputPinID,
#ifdef FLY_DEBUG
				.mReadFromDataTypeID = GetDataTypeID<std::tuple_element_t<Index, TupleType>>()
#endif
			};

			pinType.GetSetPinValueFunction().Invoke(setPinValueData, aContext);

			SetOutputValues<Index + 1>(aOutputValues, aOutputPinIDs, aContext);
		}
	}

	template<typename... Args>
	std::tuple<ReferenceWrapper<Args>...> CreateInputPack(const std::vector<PinID>& aPinIDs, const InternalExecutionContext& aContext)
	{
		std::tuple<ReferenceWrapper<Args>...> tuple;

		[&] <std::size_t... Index>(std::index_sequence<Index...>)
		{
			([&]
				{
					Pin& pin = aContext.mNodeData.mNodeRef.GetNodeGraph().GetPin(aPinIDs[Index]);
					std::get<Index>(tuple) = ReferenceWrapper<Args>(*reinterpret_cast<Args*>(pin.GetDataPtr().Get()));
				}(), ...);
		}(std::make_index_sequence<sizeof...(Args)>{});

		return tuple;
	}

	// Maybe we can use fold expression to call function without std::tuple
	template<typename... InputTypes>
	void CallInternalTest(const auto& aCallable, InternalExecutionContext& aContext)
	{
		const Node& node = aContext.mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext.mNodeData.mNodeRef.GetNodeID());
		[&] <size_t... Index>(std::index_sequence<Index...>)
		{
			aCallable(std::ref(*reinterpret_cast<InputTypes*>(aContext.mNodeData.mNodeRef.GetNodeGraph().GetPin(node.GetInputPins()[Index]).GetDataPtr().Get()))...);
		}(std::make_index_sequence<sizeof...(InputTypes)>{});
	}

	// For returning void
	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
		requires(sizeof...(OutputTypes) == 0)
	void CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{
		const Node& node = aContext.mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext.mNodeData.mNodeRef.GetNodeID());
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());

		Callable& callable = aContext.mFoundationMemoryPool->At<Callable>(nodeType.GetFunctionMemoryID());

		std::tuple<ReferenceWrapper<InputTypes>...> inputTuple = CreateInputPack<InputTypes...>(node.GetInputPins(), aContext);

		if constexpr (TakesExecutionContext && TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
			};

			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.mExecutionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext, nodeState), inputTuple));
		}
		else if constexpr (TakesExecutionContext)
		{
			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.mExecutionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext), inputTuple));
		}
		else if constexpr (TakesNodeState && TakesInternalExecutionContext)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(&aContext, nodeState), inputTuple));

		}
		else if constexpr (TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(nodeState), inputTuple));
		}
		else if constexpr (TakesInternalExecutionContext)
		{
			std::apply(callable, std::tuple_cat(std::forward_as_tuple(&aContext), inputTuple));
		}
		else
		{
			CallInternalTest(callable, aContext);
			//return std::apply(callable, inputTuple);
		}

	}

	// For not returning void
	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	std::tuple<OutputTypes...> CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		const Node& node = aContext.mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext.mNodeData.mNodeRef.GetNodeID());
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());

		Callable& callable = aContext.mFoundationMemoryPool->At<Callable>(nodeType.GetFunctionMemoryID());

		std::tuple<ReferenceWrapper<InputTypes>...> inputTuple = CreateInputPack<InputTypes...>(node.GetInputPins(), aContext);

		if constexpr (TakesExecutionContext && TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
			};

			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.mExecutionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext, nodeState), inputTuple));
		}
		else if constexpr (TakesExecutionContext)
		{
			NodeExecutionContext<NodeExecutionContextType> executionContext
			{
				*reinterpret_cast<const NodeExecutionContextType*>(aContext.mExecutionContext)
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(executionContext), inputTuple));
		}
		else if constexpr (TakesNodeState && TakesInternalExecutionContext)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
			};
			return std::apply(callable, std::tuple_cat(std::forward_as_tuple(&aContext, nodeState), inputTuple));

		}
		else if constexpr (TakesNodeState)
		{
			NodeState<NodeStateDataType> nodeState
			{
				aContext.mNodeGraphInstance->GetNodeState<NodeStateDataType>(aContext.mNodeData.mNodeRef.GetNodeID())
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
	ExecuteNodeFunction CreateExecuteNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		constexpr bool HasInputs = !EmptyParams<InputTypes...>;

		return [](const NodeExecutionData& aNodeExecutionData, InternalExecutionContext& aContext) -> void
			{

				[[maybe_unused]] const Node& node = aNodeExecutionData.mNodeRef.GetNodeGraph().GetNode(aNodeExecutionData.mNodeRef.GetNodeID());

				// Evaluate input values
				if constexpr (HasInputs)
				{
					EvaluateInputValues(node.GetSplitInputPins(), aContext);
				}

				// Set current node data before calling function
				aContext.mNodeData = aNodeExecutionData;

				// Call function and retrieve output values
				if constexpr (sizeof...(OutputTypes) > 0)
				{
					std::tuple<OutputTypes...> outputValues = CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext,
						NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});

					// Set output of function
					SetOutputValues(std::forward<std::tuple<OutputTypes...>>(std::move(outputValues)), node.GetOutputPins(), aContext);
				}
				else
				{
					CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext,
						NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});
				}
			};
	}

	template<typename First, typename... Rest>
	auto GetFirstParamAsTuple(const void* aPtr)
	{
		const First& value = *reinterpret_cast<const First*>(aPtr);
		return std::tuple<First>{ value };
	}

	template<typename First, typename... Rest>
	decltype(auto) GetRestAsTuple(const void* aRestTuple)
	{
		return *reinterpret_cast<const std::tuple<Rest...>*>(aRestTuple);
	}

	template<bool TakesInternalExecutionContext, typename Callable, typename... InputTypes>
	FastExecuteNodeFunction CreateFastExecuteNodeFunction(Callable)
	{
		//using OutputType = GetOutputType<Callable>;
		return []([[maybe_unused]] InternalExecutionContext& aContext, const MemoryPool& aFoundationMemoryPool, const NodeType& aNodeType, const void* aMainInput, const void* aInputTuple, void* aOutputValue) -> void
			{
				aMainInput;
				aMainInput;
				aInputTuple;
				aOutputValue;
				aFoundationMemoryPool;
				aNodeType;



				auto firstTuple = GetFirstParamAsTuple<InputTypes...>(aMainInput);
				auto restTuple = GetRestAsTuple<InputTypes...>(aInputTuple);

				if constexpr (false)
				{

					auto inputTuple = std::tuple_cat(std::make_tuple(&aContext), firstTuple, restTuple);

					const Callable& callable = aFoundationMemoryPool.At<Callable>(aNodeType.GetFunctionMemoryID());

					using OutputType = decltype(std::apply(callable, inputTuple));
					if constexpr (!std::same_as<OutputType, void>)
					{

						OutputType outputValue = std::apply(callable, inputTuple);


						OutputType& output = *reinterpret_cast<OutputType*>(aOutputValue);
						output = outputValue;
					}
				}
				else
				{
					auto inputTuple = std::tuple_cat(firstTuple, restTuple);

					const Callable& callable = aFoundationMemoryPool.At<Callable>(aNodeType.GetFunctionMemoryID());

					using OutputType = decltype(std::apply(callable, inputTuple));
					if constexpr (!std::same_as<OutputType, void>)
					{

						OutputType outputValue = std::apply(callable, inputTuple);


						OutputType& output = *reinterpret_cast<OutputType*>(aOutputValue);
						output = outputValue;
					}
				}
			};
	}


}