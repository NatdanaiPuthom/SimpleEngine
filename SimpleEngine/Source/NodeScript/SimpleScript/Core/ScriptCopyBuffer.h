#pragma once
#include "ScriptDefines.h"

namespace SCR
{
	struct PinCopy
	{
		MemoryPoolID memoryID = InvalidID<MemoryPoolID>();
	};

	struct NodeCopy
	{
		MemoryPool data;
		std::vector<PinCopy> inputPinCopies;
		std::vector<PinCopy> outputPinCopies;
		NodeTypeID typeID = InvalidID<NodeTypeID>();
		ScriptVec2 diffFromAvg;
	};

	class NodeGraph;

	struct CopyBuffer
	{
		std::vector<NodeCopy> nodes;
	};
}