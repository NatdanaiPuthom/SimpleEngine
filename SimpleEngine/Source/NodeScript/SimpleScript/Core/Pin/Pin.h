#pragma once
#include "../ScriptDefines.h"
#include <vector>

namespace SCR
{

	struct Pin
	{
		NodeID nodeID;
		PinTypeID typeID;
		void* dataPtr;
		std::vector<PinID> connectedPinIDs;
		std::vector<Pin> subPins;
	};

	struct Link
	{
		PinID inputPinID = InvalidID<PinID>();
		PinID outputPinID = InvalidID<PinID>();
		bool isDestroyed = false;

		operator bool() const
		{
			return inputPinID != InvalidID<PinID>() && outputPinID != InvalidID<PinID>();
		}
	};

	inline bool operator==(const Link& aLink1, const Link& aLink2)
	{
		return aLink1.inputPinID == aLink2.inputPinID && aLink1.outputPinID == aLink2.outputPinID;
	}
}