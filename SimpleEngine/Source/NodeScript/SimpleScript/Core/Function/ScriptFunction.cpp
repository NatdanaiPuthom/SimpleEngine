#include "ScriptFunction.h"
#include "NodeTypeRegistry.h"

namespace SCR
{

	static Flow CallerNode(Flow, const InternalExecutionContext* aContext)
	{
		const Node& callerNode = ScriptProxy::GetNode(aContext->script, aContext->GetNodeData().currentNodeID);

		const FunctionID functionID = NodeTypeManager::GetFunctionID(callerNode.typeID);
		const Function& function = NodeTypeManager::GetFunction(functionID);
		const Node& inputNode = ScriptProxy::GetNode(aContext->script, function.GetInputNodeID());

		CopyPinData(*aContext, inputNode.outputPins, callerNode.inputPins, 1);

		ScriptProxy::GetNodeExecutor(aContext->script).Push({ function.GetInputNodeID(), eNodeTriggerReason::Flow });

		return true;
	}

	static Flow InputNode()
	{
		return true;
	}

	static Undefined OutputNode(Flow, const InternalExecutionContext* aContext)
	{
		const Node& outputNode = aContext->GetCurrentNode();

		const FunctionID functionID = NodeTypeManager::GetFunctionID(outputNode.typeID);
		const Function& function = NodeTypeManager::GetFunction(functionID);
		function;

		// TODO: Fix node lookup
		const Node& callerNode = ScriptProxy::GetNode(aContext->script, 0);

		CopyPinData(*aContext, callerNode.outputPins, outputNode.inputPins, 1);

		return Undefined();
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