#include "FlyFunctionFacade.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{
	FunctionFacade::FunctionFacade()
		: mFunctionID(InvalidID<FunctionID>())
	{
	}

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

	void FunctionFacade::SetName(const std::string_view aName, CommandTracker* aCommandTracker)
	{
		Fly::SetFunctionName(*this, aName, aCommandTracker);
	}

	void FunctionFacade::AddPin(DataTypeFacade aDataTypeFacade, eFlowType aFlowType, std::string_view aName, CommandTracker* aCommandTracker)
	{
		AddPinToFunction(*this, aDataTypeFacade, aFlowType, aName, aCommandTracker);
	}

	void FunctionFacade::SetPinNameAtIndex(std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		SetPinNameAtIndexFunction(*this, aName, aIndex, aFlowType, aCommandTracker);
	}

	void FunctionFacade::SetPinDataTypeAtIndex(DataTypeFacade aDataTypeFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker)
	{
		SetPinDataTypeAtIndexFunction(*this, aDataTypeFacade, aIndex, aFlowType, aCommandTracker);
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
		return Global::GetNodeTypeManager().GetFunction(mFunctionID);
	}

	const Function& FunctionFacade::GetFunction() const
	{
		return Global::GetNodeTypeManager().GetFunction(mFunctionID);
	}
}
