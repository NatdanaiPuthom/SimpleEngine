#include "FlyFunctionView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"

namespace FLY_NAMESPACE
{
	FunctionView::FunctionView()
		: mFunctionID(InvalidID<FunctionID>())
	{
	}

	FunctionView::FunctionView(const FunctionID aFunctionID)
		: mFunctionID(aFunctionID)
	{
	}

	const std::string& FunctionView::GetName() const
	{
		return GetFunction().mName;
	}

	NodeView FunctionView::GetInputNode() const
	{
		const Function& function = GetFunction();
		return NodeView(function.mInputNodeID, NodeGraphView(*this));
	}

	NodeView FunctionView::GetOutputNode() const
	{
		const Function& function = GetFunction();
		return NodeView(function.mOutputNodeID, NodeGraphView(*this));
	}

	NodeTypeView FunctionView::GetCallerNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mCallerNodeTypeID);
	}

	NodeTypeView FunctionView::GetInputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mInputNodeTypeID);
	}

	NodeTypeView FunctionView::GetOutputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mOutputNodeTypeID);
	}

	NodeGraphView FunctionView::GetNodeGraph()
	{
		return NodeGraphView(*this);
	}

	FunctionID FunctionView::GetID() const
	{
		return mFunctionID;
	}

	FunctionView::operator bool() const
	{
		return mFunctionID != InvalidID<FunctionID>();
	}

	bool FunctionView::operator==(const FunctionView& aOther) const
	{
		return mFunctionID == aOther.mFunctionID;
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
