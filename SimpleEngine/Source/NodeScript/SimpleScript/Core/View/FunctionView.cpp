#include "FunctionView.h"
#include "../Global/ScriptGlobal.h"
#include "../Node/NodeTypeManager.h"

namespace SCR
{

	FunctionView::FunctionView(const FunctionID aFunctionID)
		: myFunctionID(aFunctionID)
	{
	}

	const std::string& FunctionView::GetName() const
	{
		return GetFunction().GetName();
	}

	NodeView FunctionView::GetInputNode() const
	{
		const Function& function = GetFunction();
		return NodeView(function.GetInputNodeID(), function.GetNodeGraph());
	}

	NodeView FunctionView::GetOutputNode() const
	{
		const Function& function = GetFunction();
		return NodeView(function.GetOutputNodeID(), function.GetNodeGraph());
	}

	NodeTypeView FunctionView::GetCallerNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.GetCallerNodeTypeID());
	}

	NodeTypeView FunctionView::GetInputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.GetInputNodeTypeID());
	}

	NodeTypeView FunctionView::GetOutputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.GetOutputNodeTypeID());
	}

	NodeGraph& FunctionView::GetNodeGraph()
	{
		return GetFunction().GetNodeGraph();
	}

	FunctionID FunctionView::GetID() const
	{
		return myFunctionID;
	}

	Function& FunctionView::GetFunction()
	{
		return Global::GetNodeTypeManager().GetFunction(myFunctionID);
	}

	const Function& FunctionView::GetFunction() const
	{
		return Global::GetNodeTypeManager().GetFunction(myFunctionID);
	}
}
