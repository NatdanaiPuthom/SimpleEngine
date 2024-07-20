#include "FunctionView.hpp"
#include "../Global/ScriptGlobal.hpp"
#include "../Node/NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	FunctionView::FunctionView(const FunctionID aFunctionID)
		: mFunctionID(aFunctionID)
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
		return mFunctionID;
	}

	Function& FunctionView::GetFunction()
	{
		return Global::GetNodeTypeManager().GetFunction(mFunctionID);
	}

	const Function& FunctionView::GetFunction() const
	{
		return Global::GetNodeTypeManager().GetFunction(mFunctionID);
	}
}
