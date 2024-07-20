#pragma once
#include "NodeTrait.hpp"
#include "NodeRef.hpp"
#include <vector>
#include <string>

namespace SCR
{
	struct NodeRecipe
	{
		const CreateNodeSignature mCreateFunction = nullptr;
		const ExecuteNodeSignature mExecuteFunction = nullptr;
		const eNodeTrait mTraits = eNodeTrait::None;
		const EventID mEventID = InvalidID<EventID>();
		const eNodeOperatorTrait mOperatorTrait = eNodeOperatorTrait::None;
		const DataTypeID mOwnerDataTypeID = GlobalDataTypeID;
		std::vector<PinTypeID> mInputPinTypeIDs;
		std::vector<PinTypeID> mOutputPinTypeIDs;
		const MemoryPoolID mFunctionMemoryID = InvalidID<MemoryPoolID>();
		const DataTypeID mNodeStateDataTypeID = InvalidID<DataTypeID>();
	};

	struct NodeType
	{
		NodeRecipe mNodeRecipe;
		std::string mName;
		std::vector<NodeRef> mNodeRefs;
	};

	struct NodeTypeDesc
	{
		std::vector<std::string> mInputPinNames;
		std::vector<std::string> mOutputPinNames;
		bool mShowDataTypePinNames = true;
	};
}