#include "FlyFunctionProxy.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	FunctionProxy::FunctionProxy(const FunctionID aFunctionID)
		: mFunctionID(aFunctionID)
	{
	}

	const std::string& FunctionProxy::GetName() const
	{
		return GetFunction().mName;
	}

	NodeProxy FunctionProxy::GetInputNode() const
	{
		const Function& function = GetFunction();
		return NodeProxy(function.mInputNodeID, NodeGraphProxy(*this));
	}

	NodeProxy FunctionProxy::GetOutputNode() const
	{
		const Function& function = GetFunction();
		return NodeProxy(function.mOutputNodeID, NodeGraphProxy(*this));
	}

	NodeTypeProxy FunctionProxy::GetCallerNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.mCallerNodeTypeID);
	}

	NodeTypeProxy FunctionProxy::GetInputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.mInputNodeTypeID);
	}

	NodeTypeProxy FunctionProxy::GetOutputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.mOutputNodeTypeID);
	}

	NodeGraphProxy FunctionProxy::GetNodeGraph()
	{
		return NodeGraphProxy(*this);
	}

	FunctionID FunctionProxy::GetID() const
	{
		return mFunctionID;
	}

	void FunctionProxy::SetName(const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Function& function = Internal::GetNodeTypeManager().GetFunction(mFunctionID);

		function.mName = aName;
	}

	void FunctionProxy::AddPin(GenericDataTypeProxy aDataTypeProxy, eFlowType aFlowType, std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::AddPinTypeToFunction(mFunctionID, aDataTypeProxy.GetID(), aFlowType, aName, aCommandTracker); 
	}

	void FunctionProxy::SetPinNameAtIndex(std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		Internal::SetPinNameAtIndexFunction(mFunctionID, aName, aIndex, aFlowType, aCommandTracker);
	}

	void FunctionProxy::SetPinDataTypeAtIndex(GenericDataTypeProxy aDataTypeProxy, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexFunction(mFunctionID, aDataTypeProxy.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	FunctionProxy::operator bool() const
	{
		return mFunctionID != InvalidID<FunctionID>();
	}

	bool operator==(const FunctionProxy& a, const FunctionProxy& b)
	{
		return a.mFunctionID == b.mFunctionID;
	}

	Function& FunctionProxy::GetFunction()
	{
		return Internal::GetNodeTypeManager().GetFunction(mFunctionID);
	}

	const Function& FunctionProxy::GetFunction() const
	{
		return Internal::GetNodeTypeManager().GetFunction(mFunctionID);
	}
}
