#include "FlyFunction.hpp"
#include "FlyNodeTypeRegistry.hpp"
#include "FlyClass.hpp"
#include "Fly.hpp"

namespace FLY_NAMESPACE
{

	static Flow CallerNode(InternalExecutionContextPtr aContext, Flow)
	{
		const Node& callerNode = aContext->mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext->mNodeData.mNodeRef.GetNodeID());

		NodeTypeManager& nodeTypeManager = Internal::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(callerNode.GetTypeID());
		Function& function = nodeTypeManager.GetFunction(functionID);
		const Node& inputNode = function.GetNodeGraph().GetNode(Function::INPUT_NODE_ID);

		CopyPinData(*aContext, inputNode.GetOutputPins(), callerNode.GetInputPins(), function.GetNodeGraph(), aContext->mNodeData.mNodeRef.GetNodeGraph(), 1);

		aContext->mNodeExecutionQueue->Push(NodeExecutionData{ CreateContextualNodeRef(Function::INPUT_NODE_ID, function.GetNodeGraph()), eNodeTriggerReason::Flow });
		aContext->mNodeExecutor->GetCallStack().Push(aContext->mNodeData.mNodeRef);

		return Flow(true);
	}

	static Flow InputNode()
	{
		return Flow(true);
	}

	static void OutputNode(InternalExecutionContextPtr aContext, Flow)
	{
		const Node& outputNode = aContext->mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext->mNodeData.mNodeRef.GetNodeID());

		const NodeTypeManager& nodeTypeManager = Internal::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(outputNode.GetTypeID());
		const Function& function = nodeTypeManager.GetFunction(functionID);

		const NodeRef& callerNodeRef = aContext->mNodeExecutor->GetCallStack().Pop();
		const Node& callerNode = callerNodeRef.GetNodeGraph().GetNode(callerNodeRef.GetNodeID());

		CopyPinData(*aContext, callerNode.GetOutputPins(), outputNode.GetInputPins(), callerNodeRef.GetNodeGraph(), function.GetNodeGraph(), 1);
	}

	Function::Function(std::string_view aName)
		: mName(aName)
	{
		mCallerNodeTypeID = RegisterSystemNodeType(CallerNode, NodeCreationData{ .mName = "Function/Call Function" });
		mInputNodeTypeID = RegisterSystemNodeType(InputNode, NodeCreationData{ .mName = "Function/Input Function" });
		mOutputNodeTypeID = RegisterSystemNodeType(OutputNode, NodeCreationData{ .mName = "Function/Output Function" });
	}

	const std::string& Function::GetName() const
	{
		return mName;
	}

	void Function::SetName(std::string aName)
	{
		mName = std::move(aName);
	}
}