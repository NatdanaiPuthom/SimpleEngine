#include "FlyFunction.hpp"
#include "FlyNodeTypeRegistry.hpp"
#include "FlyClass.hpp"
#include "Fly.hpp"

namespace FLY_NAMESPACE
{

	static Flow CallerNode(InternalExecutionContextPtr aContext, Flow)
	{
		const Node& callerNode = aContext->mNodeData.mNodeRef.GetNodeGraph().mNodes[aContext->mNodeData.mNodeRef.GetNodeID()];

		NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(callerNode.mTypeID);
		Function& function = nodeTypeManager.GetFunction(functionID);
		const Node& inputNode = function.mNodeGraph.mNodes[function.mInputNodeID];

		CopyPinData(*aContext, inputNode.mOutputPins, callerNode.mInputPins, function.mNodeGraph, aContext->mNodeData.mNodeRef.GetNodeGraph(), 1);

		aContext->mExecutionQueue->Push(NodeExecutionData{ CreateContextualNodeRef(function.mInputNodeID, function.mNodeGraph), eNodeTriggerReason::Flow});
		aContext->mNodeExecutor->GetCallStack().Push(aContext->mNodeData.mNodeRef);

		return Flow(true);
	}

	static Flow InputNode()
	{
		return Flow(true);
	}

	static void OutputNode(InternalExecutionContextPtr aContext, Flow)
	{
		const Node& outputNode = aContext->mNodeData.mNodeRef.GetNodeGraph().mNodes[aContext->mNodeData.mNodeRef.GetNodeID()];

		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(outputNode.mTypeID);
		const Function& function = nodeTypeManager.GetFunction(functionID);

		const NodeRef& callerNodeRef = aContext->mNodeExecutor->GetCallStack().Pop();
		const Node& callerNode = callerNodeRef.GetNodeGraph().mNodes[callerNodeRef.GetNodeID()];

		CopyPinData(*aContext, callerNode.mOutputPins, outputNode.mInputPins, callerNodeRef.GetNodeGraph(), function.mNodeGraph, 1);
	}

	Function::Function(std::string_view aName)
		: mName(aName)
	{
		mCallerNodeTypeID = RegisterSystemNodeType(CallerNode, NodeCreationData{ .mName = "Function/Call Function" });
		mInputNodeTypeID = RegisterSystemNodeType(InputNode, NodeCreationData{ .mName = "Function/Input Function" });
		mOutputNodeTypeID = RegisterSystemNodeType(OutputNode, NodeCreationData{ .mName = "Function/Output Function" });

		//std::tuple<ReferenceWrapper<const InternalExecutionContext*>, ReferenceWrapper<Flow>> t{};

		//std::apply(OutputNode, t);

		/*std::tuple<ReferenceWrapper<Flow>> t;
		InternalExecutionContext i;

		std::apply(OutputNode, std::tuple_cat(std::forward_as_tuple(&i), t));*/

	}

	Function::~Function() = default;

}