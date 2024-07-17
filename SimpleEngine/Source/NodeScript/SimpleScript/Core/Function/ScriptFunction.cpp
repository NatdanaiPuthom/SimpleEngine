#include "ScriptFunction.h"
#include "NodeTypeRegistry.h"
#include "Script.h"
#include "ScriptModifier.h"

namespace SCR
{

	static Flow CallerNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& callerNode = aContext->GetCurrentNode();

		const FunctionID functionID = NodeTypeManager::GetInstance().GetFunctionID(callerNode.typeID);
		Function& function = NodeTypeManager::GetInstance().GetFunction(functionID);
		function.SetCaller({ aContext->GetNodeData().nodeRef.nodeID, aContext->nodeData.nodeRef.nodeGraph });
		const Node& inputNode = ScriptProxy::GetNode(function.GetNodeGraph(), function.GetInputNodeID());

		CopyPinData(*aContext, inputNode.outputPins, callerNode.inputPins, function.GetNodeGraph(), *aContext->nodeData.nodeRef.nodeGraph, 1);

		aContext->executionQueue->Push({ NodeRef{function.GetInputNodeID(), &function.GetNodeGraph() }, eNodeTriggerReason::Flow});

		return Flow(true);
	}

	static Flow InputNode()
	{
		return Flow(true);
	}

	static Wildcard OutputNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& outputNode = aContext->GetCurrentNode();

		const FunctionID functionID = NodeTypeManager::GetInstance().GetFunctionID(outputNode.typeID);
		const Function& function = NodeTypeManager::GetInstance().GetFunction(functionID);

		const Function::FunctionCaller& caller = function.GetCaller();

		// TODO: Fix node lookup
		const Node& callerNode = ScriptProxy::GetNode(*caller.nodeGraph, caller.nodeID);

		CopyPinData(*aContext, callerNode.outputPins, outputNode.inputPins, *caller.nodeGraph, function.GetNodeGraph(), 1);

		return Wildcard();
	}

	Function::Function(std::string_view aName)
		: myName(aName)
		, myNodeGraph(eNodeGraphType::Function)
	{
		myCallerNodeTypeID = RegisterSystemNodeType(CallerNode, "Function/Call Function");
		myInputNodeTypeID = RegisterSystemNodeType(InputNode, "Function/Input Function");
		myOutputNodeTypeID = RegisterSystemNodeType(OutputNode, "Function/Output Function");

		myInputNodeID = Modify::CreateNode(myNodeGraph, myInputNodeTypeID);
		myOutputNodeID = Modify::CreateNode(myNodeGraph, myOutputNodeTypeID);
	}

	Function::~Function() = default;

}