#include "FlyFunctionFacade.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	FunctionFacade::FunctionFacade(const FunctionID aFunctionID)
		: mFunctionID(aFunctionID)
	{
	}

	const std::string& FunctionFacade::GetName() const
	{
		return GetFunction().mName;
	}

	NodeFacade FunctionFacade::GetInputNodeFacade() const
	{
		const Function& function = GetFunction();
		return NodeFacade(function.mInputNodeID, NodeGraphFacade(*this));
	}

	NodeFacade FunctionFacade::GetOutputNodeFacade() const
	{
		const Function& function = GetFunction();
		return NodeFacade(function.mOutputNodeID, NodeGraphFacade(*this));
	}

	NodeTypeFacade FunctionFacade::GetCallerNodeTypeFacade() const
	{
		const Function& function = GetFunction();
		return NodeTypeFacade(function.mCallerNodeTypeID);
	}

	NodeTypeFacade FunctionFacade::GetInputNodeTypeFacade() const
	{
		const Function& function = GetFunction();
		return NodeTypeFacade(function.mInputNodeTypeID);
	}

	NodeTypeFacade FunctionFacade::GetOutputNodeTypeFacade() const
	{
		const Function& function = GetFunction();
		return NodeTypeFacade(function.mOutputNodeTypeID);
	}

	NodeGraphFacade FunctionFacade::GetNodeGraphFacade()
	{
		return NodeGraphFacade(*this);
	}

	FunctionID FunctionFacade::GetID() const
	{
		return mFunctionID;
	}

	void FunctionFacade::SetName(const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Function& function = Internal::GetNodeTypeManager().GetFunction(mFunctionID);

		function.mName = aName;
	}

	void FunctionFacade::AddPin(DataTypeFacade aDataTypeFacade, eFlowType aFlowType, std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::AddPinToFunction(mFunctionID, aDataTypeFacade.GetID(), aFlowType, aName, aCommandTracker); 
	}

	void FunctionFacade::SetPinNameAtIndex(std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		Internal::SetPinNameAtIndexFunction(mFunctionID, aName, aIndex, aFlowType, aCommandTracker);
	}

	void FunctionFacade::SetPinDataTypeAtIndex(DataTypeFacade aDataTypeFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexFunction(mFunctionID, aDataTypeFacade.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	FunctionFacade::operator bool() const
	{
		return mFunctionID != InvalidID<FunctionID>();
	}

	bool operator==(const FunctionFacade& a, const FunctionFacade& b)
	{
		return a.mFunctionID == b.mFunctionID;
	}

	Function& FunctionFacade::GetFunction()
	{
		return Internal::GetNodeTypeManager().GetFunction(mFunctionID);
	}

	const Function& FunctionFacade::GetFunction() const
	{
		return Internal::GetNodeTypeManager().GetFunction(mFunctionID);
	}
}
