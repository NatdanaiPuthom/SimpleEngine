#pragma once
#include "FlyNodeTrait.hpp"
#include "FlyNodeRef.hpp"
#include <vector>
#include <string>

namespace FLY_NAMESPACE
{
	struct NodeRecipe final
	{
		const CreateNodeSignature mCreateFunction = nullptr;
		const ExecuteNodeSignature mExecuteFunction = nullptr;
		const eNodeTrait mTraits = eNodeTrait::None;
		const EventID mEventID = InvalidID<EventID>();
		const DataTypeID mOwnerDataTypeID = InvalidID<DataTypeID>();
		std::vector<PinTypeID> mInputPinTypeIDs;
		std::vector<PinTypeID> mOutputPinTypeIDs;
		const MemoryPoolID mFunctionMemoryID = InvalidID<MemoryPoolID>();
		const DataTypeID mNodeStateDataTypeID = InvalidID<DataTypeID>();
		std::string mName;
		const eNodeOperatorTrait mOperatorTrait = eNodeOperatorTrait::None;
	};

	struct NodeType final
	{
		NodeRecipe mNodeRecipe;
		std::vector<NodeRef> mNodeRefs;
	};

	struct NodeTypeDesc final
	{
		std::vector<std::string> mInputPinNames;
		std::vector<std::string> mOutputPinNames;
		std::vector<std::any> mDefaultValues;
		bool mShowDataTypePinNames = true;
	};
}