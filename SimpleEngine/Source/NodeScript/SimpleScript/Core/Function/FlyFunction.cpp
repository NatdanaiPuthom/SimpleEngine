#include "FlyFunction.hpp"
#include "FlyNodeTypeRegistry.hpp"
#include "FlyClass.hpp"
#include "Fly.hpp"

namespace FLY_NAMESPACE
{

	static Flow CallerNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& callerNode = aContext->mNodeData.mNodeRef.mNodeGraph->mNodes[aContext->mNodeData.mNodeRef.mNodeID];

		NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(callerNode.mTypeID);
		Function& function = nodeTypeManager.GetFunction(functionID);
		const Node& inputNode = function.mNodeGraph.mNodes[function.mInputNodeID];

		CopyPinData(*aContext, inputNode.mOutputPins, callerNode.mInputPins, function.mNodeGraph, *aContext->mNodeData.mNodeRef.mNodeGraph, 1);

		aContext->mExecutionQueue->Push(NodeExecutionData{ NodeRef{ function.mInputNodeID, &function.mNodeGraph }, eNodeTriggerReason::Flow});
		aContext->mNodeExecutor->GetCallStack().Push(aContext->mNodeData.mNodeRef);

		return Flow(true);
	}

	static Flow InputNode()
	{
		return Flow(true);
	}

	static Wildcard OutputNode(const InternalExecutionContext* aContext, Flow)
	{
		const Node& outputNode = aContext->mNodeData.mNodeRef.mNodeGraph->mNodes[aContext->mNodeData.mNodeRef.mNodeID];

		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const FunctionID functionID = nodeTypeManager.GetFunctionID(outputNode.mTypeID);
		const Function& function = nodeTypeManager.GetFunction(functionID);

		const NodeRef& callerNodeRef = aContext->mNodeExecutor->GetCallStack().Pop();
		const Node& callerNode = callerNodeRef.mNodeGraph->mNodes[callerNodeRef.mNodeID];

		CopyPinData(*aContext, callerNode.mOutputPins, outputNode.mInputPins, *callerNodeRef.mNodeGraph, function.mNodeGraph, 1);

		return Wildcard();
	}

	Function::Function(std::string_view aName)
		: mName(aName)
	{
		mCallerNodeTypeID = RegisterSystemNodeType(CallerNode, "Function/Call Function");
		mInputNodeTypeID = RegisterSystemNodeType(InputNode, "Function/Input Function");
		mOutputNodeTypeID = RegisterSystemNodeType(OutputNode, "Function/Output Function");

		mInputNodeID = Internal::CreateNode(&mNodeGraph, mInputNodeTypeID);
		mOutputNodeID = Internal::CreateNode(&mNodeGraph, mOutputNodeTypeID);
	}

	Function::~Function() = default;

}