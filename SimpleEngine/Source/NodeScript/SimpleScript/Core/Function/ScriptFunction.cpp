#include "ScriptFunction.hpp"
#include "NodeTypeRegistry.hpp"
#include "FlyClass.hpp"
#include "ScriptModifier.hpp"

namespace SCR
{

	static Flow CallerNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& callerNode = aContext->GetCurrentNode();

		NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(callerNode.mTypeID);
		Function& function = nodeTypeManager.GetFunction(functionID);
		function.SetCaller({ aContext->GetNodeData().mNodeRef.mNodeID, aContext->mNodeData.mNodeRef.mNodeGraph });
		const Node& inputNode = ScriptProxy::GetNode(function.GetNodeGraph(), function.GetInputNodeID());

		CopyPinData(*aContext, inputNode.mOutputPins, callerNode.mInputPins, function.GetNodeGraph(), *aContext->mNodeData.mNodeRef.mNodeGraph, 1);

		aContext->mExecutionQueue->Push(NodeExecutionData{ NodeRef{function.GetInputNodeID(), &function.GetNodeGraph() }, eNodeTriggerReason::Flow});

		return Flow(true);
	}

	static Flow InputNode()
	{
		return Flow(true);
	}

	static Wildcard OutputNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& outputNode = aContext->GetCurrentNode();

		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(outputNode.mTypeID);
		const Function& function = nodeTypeManager.GetFunction(functionID);

		const NodeRef& caller = function.GetCaller();

		// TODO: Fix node lookup
		const Node& callerNode = ScriptProxy::GetNode(*caller.mNodeGraph, caller.mNodeID);

		CopyPinData(*aContext, callerNode.mOutputPins, outputNode.mInputPins, *caller.mNodeGraph, function.GetNodeGraph(), 1);

		return Wildcard();
	}

	Function::Function(std::string_view aName)
		: mName(aName)
		, mNodeGraph(eNodeGraphType::Function)
	{
		mCallerNodeTypeID = RegisterSystemNodeType(CallerNode, "Function/Call Function");
		mInputNodeTypeID = RegisterSystemNodeType(InputNode, "Function/Input Function");
		mOutputNodeTypeID = RegisterSystemNodeType(OutputNode, "Function/Output Function");

		mInputNodeID = CreateNode(mNodeGraph, mInputNodeTypeID).GetID();
		mOutputNodeID = CreateNode(mNodeGraph, mOutputNodeTypeID).GetID();
	}

	Function::~Function() = default;

}