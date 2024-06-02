#include "ScriptFunction.h"
#include "NodeTypeRegistry.h"
#include "Script.h"

namespace SCR
{

	static Flow CallerNode(Flow, const InternalExecutionContext* aContext)
	{
		const Node& callerNode = aContext->GetCurrentNode();

		const FunctionID functionID = NodeTypeManager::GetFunctionID(callerNode.typeID);
		Function& function = NodeTypeManager::GetFunction(functionID);
		function.SetCaller({ aContext->GetNodeData().nodeRef.nodeID, aContext->nodeData.nodeRef.nodeGraph });
		const Node& inputNode = ScriptProxy::GetNode(*aContext->nodeData.nodeRef.nodeGraph, function.GetInputNodeID());

		CopyPinData(*aContext, inputNode.outputPins, callerNode.inputPins, 1);

		aContext->executionQueue->Push({ NodeRef{function.GetInputNodeID(), &ScriptProxy::GetEventGraph(*aContext->script) }, eNodeTriggerReason::Flow });

		return true;
	}

	static Flow InputNode()
	{
		return true;
	}

	static Wildcard OutputNode(Flow, const InternalExecutionContext* aContext)
	{
		const Node& outputNode = aContext->GetCurrentNode();

		const FunctionID functionID = NodeTypeManager::GetFunctionID(outputNode.typeID);
		const Function& function = NodeTypeManager::GetFunction(functionID);
		function;

		const auto& caller = function.GetCaller();

		// TODO: Fix node lookup
		const Node& callerNode = ScriptProxy::GetNode(*caller.nodeGraph, caller.nodeID);

		CopyPinData(*aContext, callerNode.outputPins, outputNode.inputPins, 1);

		return Wildcard();
	}

	Function::Function(const std::string& aName)
		: myName(aName)
	{
		myCallerNodeTypeID = RegisterSystemNodeType(CallerNode, "Function/Call Function");
		myInputNodeTypeID = RegisterSystemNodeType(InputNode, "Function/Input Function");
		myOutputNodeTypeID = RegisterSystemNodeType(OutputNode, "Function/Output Function");
	}

	Function::~Function() = default;

}