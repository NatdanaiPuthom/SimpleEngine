#include "FlyFunctionView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"

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

	NodeView FunctionView::GetInputNodeView() const
	{
		const Function& function = GetFunction();
		return NodeView(function.mInputNodeID, NodeGraphView(*this));
	}

	NodeView FunctionView::GetOutputNodeView() const
	{
		const Function& function = GetFunction();
		return NodeView(function.mOutputNodeID, NodeGraphView(*this));
	}

	NodeTypeView FunctionView::GetCallerNodeTypeView() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mCallerNodeTypeID);
	}

	NodeTypeView FunctionView::GetInputNodeTypeView() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mInputNodeTypeID);
	}

	NodeTypeView FunctionView::GetOutputNodeTypeView() const
	{
		const Function& function = GetFunction();
		return NodeTypeView(function.mOutputNodeTypeID);
	}

	NodeGraphView FunctionView::GetNodeGraphView()
	{
		return NodeGraphView(*this);
	}

	FunctionID FunctionView::GetID() const
	{
		return mFunctionID;
	}

	void FunctionView::SetName(const std::string_view aName, CommandTracker* aCommandTracker)
	{
		Fly::SetFunctionName(*this, aName, aCommandTracker);
	}

	FunctionView::operator bool() const
	{
		return mFunctionID != InvalidID<FunctionID>();
	}

	bool operator==(const FunctionView& a, const FunctionView& b)
	{
		return a.mFunctionID == b.mFunctionID;
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
