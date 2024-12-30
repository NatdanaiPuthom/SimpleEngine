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
			const Pin& pin = aContext.mNodeData.mNodeRef.GetNodeGraph().mPins[outputPinID];
			const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pin.mTypeID);
			assert(pinType.mFlowType == eFlowType::Output);

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

			pinType.mSetPinValueFunction(setPinValueData, aContext);

			SetOutputValues<Index + 1>(aOutputValues, aOutputPinIDs, aContext);
		}
	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
		requires(sizeof...(OutputTypes) == 0)
	void CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{
		const Node& node = aContext.mNodeData.mNodeRef.GetNodeGraph().mNodes[aContext.mNodeData.mNodeRef.GetNodeID()];
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.mTypeID);

		MemoryPool& foundationMemoryPool = Internal::GetMemoryPool();

		MemoryPoolID functionMemoryID = nodeType.mNodeRecipe.mFunctionMemoryID;
		Callable& callable = foundationMemoryPool.At<Callable>(functionMemoryID);

		std::tuple<ReferenceWrapper<InputTypes>...> inputTuple = CreateInputPack<InputTypes...>(node.mInputPins, aContext);

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
			return std::apply(callable, inputTuple);
		}

	}

	template<bool TakesExecutionContext, bool TakesNodeState, bool TakesInternalExecutionContext, typename NodeExecutionContextType, typename NodeStateDataType, typename Callable, typename... OutputTypes, typename... InputTypes>
	std::tuple<OutputTypes...> CallFunction(InternalExecutionContext& aContext, TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		const Node& node = aContext.mNodeData.mNodeRef.GetNodeGraph().mNodes[aContext.mNodeData.mNodeRef.GetNodeID()];
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.mTypeID);

		MemoryPool& foundationMemoryPool = Internal::GetMemoryPool();

		MemoryPoolID functionMemoryID = nodeType.mNodeRecipe.mFunctionMemoryID;
		Callable& callable = foundationMemoryPool.At<Callable>(functionMemoryID);

		std::tuple<ReferenceWrapper<InputTypes>...> inputTuple = CreateInputPack<InputTypes...>(node.mInputPins, aContext);

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
	ExecuteNodeSignature CreateExecuteNodeFunction(TypeList<OutputTypes...>, TypeList<InputTypes...>)
	{

		constexpr bool HasInputs = !EmptyParameterPack<InputTypes...>;

		return [](const NodeExecutionData& aNodeExecutionData, InternalExecutionContext& aContext) -> void
			{

				[[maybe_unused]] const Node& node = aNodeExecutionData.mNodeRef.GetNodeGraph().mNodes[aNodeExecutionData.mNodeRef.GetNodeID()];



				// Evaluate input values
				if constexpr (HasInputs)
				{
					EvaluateInputValues(node.mSplitInputPins, aContext);
				}

				// Set current node data before calling function
				aContext.mNodeData = aNodeExecutionData;

				// Call function and retrieve output values
				if constexpr (sizeof...(OutputTypes) > 0)
				{
					std::tuple<OutputTypes...> outputValues = CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext,
						NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});

					// Set output of function
					SetOutputValues(std::forward<std::tuple<OutputTypes...>>(std::move(outputValues)), node.mOutputPins, aContext);
				}
				else
				{
					CallFunction<TakesExecutionContext, TakesNodeState, TakesInternalExecutionContext,
						NodeExecutionContextType, NodeStateDataType, Callable>(aContext, TypeList<OutputTypes...>{}, TypeList<InputTypes...>{});
				}
			};
	}
}