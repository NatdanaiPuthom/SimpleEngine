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
		NodeTypeID typeID = InvalidID<NodeTypeID>();
		MemoryPool data;
		std::vector<PinCopy> inputPinCopies;
		std::vector<PinCopy> outputPinCopies;
		ScriptVec2 diffFromAvg;
	};

	struct CopyBuffer
	{
		//std::vector<NodeCopy> nodes;
		NodeGraph nodeGraph;
	};
}