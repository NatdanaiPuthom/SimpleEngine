#pragma once
#include "../FlyDefines.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class Pin final
	{
	public:

		Pin(PinTypeID aTypeID, NonOwningPtr<void> aDataPtr, NodeID aNodeID);

		[[nodiscard]] PinTypeID GetTypeID() const;
		void SetDataPtr(NonOwningPtr<void> aDataPtr);
		[[nodiscard]] OwningPtr<void> GetDataPtr() const;
		[[nodiscard]] const std::vector<PinID>& GetConnectedPinIDs() const;
		void SetSplitPinIDs(std::vector<PinID> aPinIDs);
		[[nodiscard]] const std::vector<PinID>& GetSplitPinIDs() const;
		[[nodiscard]] NodeID GetNodeID() const;
		void SetParentPinID(PinID aPinID);
		[[nodiscard]] PinID GetParentPinID() const;
		void SetIsSplit(bool aIsSplit);
		[[nodiscard]] bool IsSplit() const;

		void AddConnectedPin(PinID aPinID);
		void RemoveConnectedPin(PinID aPinID);

	private:

		PinTypeID mTypeID = InvalidID<PinTypeID>();
		OwningPtr<void> mDataPtr;
		std::vector<PinID> mConnectedPinIDs;
		std::vector<PinID> mSplitPinIDs;
		NodeID mNodeID = InvalidID<NodeID>();
		PinID mParentPinID = InvalidID<PinID>();
		bool mIsSplit = false;
	};

	inline PinTypeID Pin::GetTypeID() const
	{
		return mTypeID;
	}

	inline OwningPtr<void> Pin::GetDataPtr() const
	{
		return mDataPtr;
	}

	inline const std::vector<PinID>& Pin::GetConnectedPinIDs() const
	{
		return mConnectedPinIDs;
	}

	inline const std::vector<PinID>& Pin::GetSplitPinIDs() const
	{
		return mSplitPinIDs;
	}

	inline NodeID Pin::GetNodeID() const
	{
		return mNodeID;
	}
}