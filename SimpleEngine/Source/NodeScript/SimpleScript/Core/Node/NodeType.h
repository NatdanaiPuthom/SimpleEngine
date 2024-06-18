#pragma once
#include "NodeTrait.h"
#include "NodeRef.h"
#include <vector>
#include <string>

namespace SCR
{
	struct NodeRecipe
	{
		const CreateNodeSignature createFunction = nullptr;
		const ExecuteNodeSignature executeFunction = nullptr;
		const eNodeTrait traits = eNodeTrait::None;
		const EventID eventID = 0;
		const eNodeOperatorTrait operatorTrait = eNodeOperatorTrait::None;
		std::vector<PinTypeID> inputPinTypeIDs;
		std::vector<PinTypeID> outputPinTypeIDs;
		//const MemoryPoolID tupleMemoryID = InvalidID<MemoryPoolID>();
		const MemoryPoolID functionMemoryID = InvalidID<MemoryPoolID>();
	};

	struct NodeType
	{
		NodeRecipe nodeRecipe;
		std::string name;
		std::vector<NodeRef> nodeRefs;
	};

	struct NodeTypeDesc
	{
		std::vector<std::string> inputPinNames;
		std::vector<std::string> outputPinNames;
		bool showDataTypePinNames = true;
	};
}