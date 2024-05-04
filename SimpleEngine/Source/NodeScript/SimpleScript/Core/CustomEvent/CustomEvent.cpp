#include "CustomEvent.h"
#include "../Node/NodeTypeRegistry.h"

namespace SCR
{

	//static Node CreateExecutionNode(const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier)
	//{
	//	std::array<PinID, 1> preExistingOutpinIDs = CreateOutputPins<Flow>(aNodeID, aNodeTypeID, aModifier);
	//	std::vector<PinID> outputPinIDs = aModifier.CreateOutputPins(aNodeID, aNodeTypeID, 1);

	//	std::vector<PinID> combinedOutputPinIDs;
	//	for (PinID pinID : preExistingOutpinIDs)
	//	{
	//		combinedOutputPinIDs.push_back(pinID);
	//	}

	//	for (PinID pinID : outputPinIDs)
	//	{
	//		combinedOutputPinIDs.push_back(pinID);
	//	}

	//	return Node(aNodeTypeID, std::array<PinID, 0>{}, combinedOutputPinIDs);
	//}

	//static void ExecuteExecutionNode(const NodeID aNodeID, InternalExecutionContext& aContext)
	//{
	//	const Node& node = ScriptProxy::GetNode(aContext.script, aNodeID);

	//	SetOutputValues(std::tuple<Flow>(true), node.outputPins, aContext);
	//}

	

	//static Node CreateCallerNode(const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier)
	//{

	//	std::array<PinID, 1> preExistingInputs = CreateInputPins<Flow>(aNodeID, aNodeTypeID, aModifier);

	//	std::vector<PinID> inputPinIDs = aModifier.CreateInputPins(aNodeID, aNodeTypeID, 1);

	//	std::vector<PinID> combinedInputPinIDs;

	//	for (PinID pinID : preExistingInputs)
	//	{
	//		combinedInputPinIDs.push_back(pinID);
	//	}

	//	for (PinID pinID : inputPinIDs)
	//	{
	//		combinedInputPinIDs.push_back(pinID);
	//	}

	//	std::array<PinID, 1> outputPinIDs = CreateOutputPins<Flow>(aNodeID, aNodeTypeID, aModifier);

	//	return Node(aNodeTypeID, combinedInputPinIDs, outputPinIDs);


	//}

	//static void ExecuteCallerNode(const NodeID aNodeID, InternalExecutionContext& aContext)
	//{
	//	const Node& node = ScriptProxy::GetNode(aContext.script, aNodeID);
	//	CustomEventID customEventNodeTypeID = NodeTypeManager::GetCustomEventNodeTypeID(node.typeID);

	//	const CustomEvent& dynamicNodeType = NodeTypeManager::GetCustomEvent(customEventNodeTypeID);

	//	const NodeID callerNodeID = aNodeID;
	//	const Node& callerNode = ScriptProxy::GetNode(aContext.script, callerNodeID);
	//	const NodeType& callerNodeType = NodeTypeManager::GetNodeType(callerNode.typeID);

	//	MemoryPool& foundationMemoryPool = ScriptProxy::GetGlobalMemoryPool();

	//	MemoryPoolID tupleMemoryID = callerNodeType.nodeRecipe.tupleMemoryID;
	//	MemoryTuple<Flow>& memoryTuple = foundationMemoryPool.At<MemoryTuple<Flow>>(tupleMemoryID);


	//	EvaluateInputValues(node.inputPins, aContext);

	//	auto callerFunc = [](NodeID aCallerNodeID, const CustomEvent* aCustomEventNodeType, InternalExecutionContext* aContext, Flow) -> Flow
	//		{
	//			// Sets the values of the custom 

	//			const MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext->script);
	//			NodeExecutor& nodeExecutor = ScriptProxy::GetNodeExecutor(aContext->script);

	//			const Node& callerNode = ScriptProxy::GetNode(aContext->script, aCallerNodeID);

	//			const std::vector<NodeID>& executorNodeIDs = ScriptProxy::GetNodeIDsByNodeType(aContext->script, aCustomEventNodeType->myExecutorTypeID);
	//			for (NodeID executorNodeID : executorNodeIDs)
	//			{

	//				nodeExecutor.Push({ executorNodeID, eNodeTriggerReason::Flow });

	//				const Node& executorNode = ScriptProxy::GetNode(aContext->script, executorNodeID);

	//				for (size_t i = 1; i < executorNode.outputPins.size(); i++)
	//				{
	//					PinID outputPinID = executorNode.outputPins.at(i);

	//					const Pin& outputPin = ScriptProxy::GetPin(aContext->script, outputPinID);

	//					const PinType& outputPinType = PinTypeManager::GetPinType(outputPin.typeID);

	//					const PinID callerInputPinID = callerNode.inputPins.at(i);
	//					const Pin& callerPin = ScriptProxy::GetPin(aContext->script, callerInputPinID);
	//					const void* value = memoryPool.MemoryAt(callerPin.memoryID);


	//					outputPinType.setFunction(outputPinID, *aContext, value);
	//				}
	//			}


	//			return true;
	//		};

	//	std::tuple<Flow> outputValues = memoryTuple.Call(callerFunc, &ScriptProxy::GetScriptMemoryPool(aContext.script), ToArray<1>(node.inputPins, aContext), callerNodeID, &dynamicNodeType, &aContext);

	//	SetOutputValues(outputValues, callerNode.outputPins, aContext);
	//}

	static void CustomEventCallerNode(InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 

		NodeID callerNodeID = aContext->GetNodeData().currentNodeID;

		const Node& callerNode = ScriptProxy::GetNode(aContext->script, callerNodeID);
		CustomEventID customEventNodeTypeID = NodeTypeManager::GetCustomEventNodeTypeID(callerNode.typeID);

		const CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(customEventNodeTypeID);

		//const NodeType& callerNodeType = NodeTypeManager::GetNodeType(callerNode.typeID);

		const MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext->script);
		NodeExecutor& nodeExecutor = ScriptProxy::GetNodeExecutor(aContext->script);

		const std::vector<NodeID>& executorNodeIDs = ScriptProxy::GetNodeIDsByNodeType(aContext->script, customEvent.myExecutorTypeID);
		for (NodeID executorNodeID : executorNodeIDs)
		{

			nodeExecutor.Push({ executorNodeID, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(aContext->script, executorNodeID);

			for (size_t i = 1; i < executorNode.outputPins.size(); i++)
			{
				PinID outputPinID = executorNode.outputPins.at(i);

				const Pin& outputPin = ScriptProxy::GetPin(aContext->script, outputPinID);

				const PinType& outputPinType = PinTypeManager::GetPinType(outputPin.typeID);

				const PinID callerInputPinID = callerNode.inputPins.at(i);
				const Pin& callerPin = ScriptProxy::GetPin(aContext->script, callerInputPinID);
				const void* value = memoryPool.MemoryAt(callerPin.memoryID);


				outputPinType.setFunction(outputPinID, *aContext, value);
			}
		}
	}


	static Flow CustomEventExecutorNode()
	{
		return true;
	}

	CustomEvent::CustomEvent(const std::string& aName)
	{
		//NodeTypeRegistry::RegisterFlowNodeType(CustomEventCallerNode, "CustomEvent/Call " + aName);
		myCallerTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent | eNodeTrait::HasImplicitFlow>(CustomEventCallerNode, "CustomEvent/Call " + aName);
		myExecutorTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent>(CustomEventExecutorNode, "CustomEvent/" + aName);
		//NodeTypeRegistry::RegisterNodeType(CustomEventExecutorNode, "CustomEvent/" + aName);
		/*{

			std::vector<PinTypeID> outputPinTypes = CreatePinTypes<ePinFlowType::Output, Flow>({ "Flow" });

			NodeType t
			{
				NodeRecipe
				{
					CreateExecutionNode,
					ExecuteExecutionNode,
					eNodeTrait::CustomEvent | eNodeTrait::HasFlow,
					eNodeExecutionTrait::None,
					eNodeOperatorTrait::None,
					{},
					outputPinTypes
				},
				"CustomEvent/" + aName
			};

			myExecutorTypeID = NodeTypeManager::Register(std::move(t));
		}

		{

			MemoryTuple<Flow> memoryTuple;
			std::vector<PinTypeID> inputPinTypes = CreatePinTypes<ePinFlowType::Input, Flow>(memoryTuple, { "#T" });
			std::vector<PinTypeID> outputPinTypes = CreatePinTypes<ePinFlowType::Output, Flow>({ "#T" });

			MemoryPoolID tupleMemoryID = aFoundationMemoryPool.Allocate<MemoryTuple<Flow>>(memoryTuple);

			NodeType t
			{
				NodeRecipe
				{
					CreateCallerNode,
					ExecuteCallerNode,
					eNodeTrait::CustomEvent | eNodeTrait::HasFlow,
					eNodeExecutionTrait::None,
					eNodeOperatorTrait::None,
					inputPinTypes,
					outputPinTypes,
					tupleMemoryID
				},
				"CustomEvent/Call " + aName
			};

			myCallerTypeID = NodeTypeManager::Register(std::move(t));
		}*/
	}

	CustomEvent::~CustomEvent()
	{
	}
}