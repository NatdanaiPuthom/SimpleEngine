#include "FlyNodeType.hpp"

namespace FLY_NAMESPACE
{

	NodeType::NodeType(std::string aName, CreateNodeFunction aCreateFunction, ExecuteNodeFunction aExecuteFunction, FastExecuteNodeFunction aFastExecuteFunction,
		eNodeTrait aTraits, EventID aEventID, DataTypeID aOwnerDataTypeID, std::vector<PinTypeID> aInputPinTypeIDs, std::vector<PinTypeID> aOutputPinTypeIDs,
		MemoryPoolID aFunctionMemoryID, DataTypeID aNodeStateDataTypeID, TraitID aTraitID, eNodeOperatorTrait aOperatorTrait)
		: mName(std::move(aName))
		, mCreateFunction(aCreateFunction)
		, mExecuteFunction(aExecuteFunction)
		, mFastExecuteFunction(aFastExecuteFunction)
		, mTraits(aTraits)
		, mEventID(aEventID)
		, mOwnerDataTypeID(aOwnerDataTypeID)
		, mInputPinTypeIDs(std::move(aInputPinTypeIDs))
		, mOutputPinTypeIDs(std::move(aOutputPinTypeIDs))
		, mFunctionMemoryID(aFunctionMemoryID)
		, mNodeStateDataTypeID(aNodeStateDataTypeID)
		, mTraitID(aTraitID)
		, mOperatorTrait(aOperatorTrait)
	{
	}

	const std::string& NodeType::GetName()const
	{
		return mName;
	}

	std::vector<NodeRef>& NodeType::GetNodeRefs()
	{
		return mNodeRefs;
	}

	const std::vector<NodeRef>& NodeType::GetNodeRefs() const
	{
		return mNodeRefs;
	}

	void NodeType::SetName(std::string aName)
	{
		mName = std::move(aName);
	}
}