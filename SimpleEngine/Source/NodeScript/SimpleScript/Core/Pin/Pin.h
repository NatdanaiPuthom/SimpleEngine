#pragma once
#include "../ScriptDefines.h"
#include <vector>

namespace SCR
{

	struct Pin
	{
		const NodeID nodeID;
		const PinTypeID typeID;
		void* const dataPtr;
		//const MemoryPoolID memoryID;
		std::vector<PinID> connectedPinIDs;
	};

	struct Link
	{
		PinID inputPinID = InvalidID<PinID>();
		PinID outputPinID = InvalidID<PinID>();

		bool operator<(const Link& aLink) const
		{
			return inputPinID < aLink.inputPinID;
		}

		operator bool() const
		{
			return inputPinID != InvalidID<PinID>();
		}
	};

}