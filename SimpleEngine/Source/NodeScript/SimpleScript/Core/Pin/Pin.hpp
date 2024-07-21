#pragma once
#include "../ScriptDefines.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	struct Pin final
	{
		NodeID mNodeID;
		PinTypeID mTypeID;
		void* mDataPtr;
		std::vector<PinID> mConnectedPinIDs;
	};

	struct Link final
	{
		PinID mInputPinID = InvalidID<PinID>();
		PinID mOutputPinID = InvalidID<PinID>();
		bool mIsDestroyed = false;

		explicit operator bool() const
		{
			return mInputPinID != InvalidID<PinID>() && mOutputPinID != InvalidID<PinID>();
		}
	};

	inline bool operator==(const Link& aLink1, const Link& aLink2)
	{
		return aLink1.mInputPinID == aLink2.mInputPinID && aLink1.mOutputPinID == aLink2.mOutputPinID;
	}
}