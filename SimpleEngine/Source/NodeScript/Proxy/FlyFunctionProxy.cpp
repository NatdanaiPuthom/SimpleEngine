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
		return GetFunction().GetName();
	}

	NodeProxy FunctionProxy::GetInputNode() const
	{
		return NodeProxy(Function::INPUT_NODE_ID, NodeGraphProxy(*this));
	}

	NodeProxy FunctionProxy::GetOutputNode() const
	{
		return NodeProxy(Function::OUTPUT_NODE_ID, NodeGraphProxy(*this));
	}

	NodeTypeProxy FunctionProxy::GetCallerNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.GetCallerNodeTypeID());
	}

	NodeTypeProxy FunctionProxy::GetInputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.GetInputNodeTypeID());
	}

	NodeTypeProxy FunctionProxy::GetOutputNodeType() const
	{
		const Function& function = GetFunction();
		return NodeTypeProxy(function.GetOutputNodeTypeID());
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

		function.SetName(std::string(aName));
	}

	void FunctionProxy::AddPin(GenericDataTypeProxy aDataTypeProxy, eIODirection aIODirection, std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::AddPinTypeToFunction(mFunctionID, aDataTypeProxy.GetID(), aIODirection, aName, aCommandTracker);
	}

	void FunctionProxy::SetPinNameAtIndex(std::string_view aName, size_t aIndex, eIODirection aIODirection, CommandTracker* aCommandTracker)
	{
		Internal::SetPinNameAtIndexFunction(mFunctionID, aName, aIndex, aIODirection, aCommandTracker);
	}

	void FunctionProxy::SetPinDataTypeAtIndex(GenericDataTypeProxy aDataTypeProxy, size_t aIndex, eIODirection aIODirection, CommandTracker* aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexFunction(mFunctionID, aDataTypeProxy.GetID(), aIndex, aIODirection, aCommandTracker);
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
