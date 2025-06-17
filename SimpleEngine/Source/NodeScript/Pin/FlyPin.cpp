#include "FlyPin.hpp"

namespace FLY_NAMESPACE
{

	Pin::Pin(const PinTypeID aTypeID, const NonOwningPtr<void> aDataPtr, const NodeID aNodeID)
		: mTypeID(aTypeID)
		, mDataPtr(aDataPtr.Get())
		, mNodeID(aNodeID)
	{
	}

	void Pin::SetDataPtr(NonOwningPtr<void> aDataPtr)
	{
		mDataPtr = aDataPtr.Get();
	}

	void Pin::SetSplitPinIDs(std::vector<PinID> aPinIDs)
	{
		mSplitPinIDs = std::move(aPinIDs);
	}

	void Pin::SetParentPinID(const PinID aPinID)
	{
		mParentPinID = aPinID;
	}

	PinID Pin::GetParentPinID() const
	{
		return mParentPinID;
	}

	void Pin::SetIsSplit(bool aIsSplit)
	{
		mIsSplit = aIsSplit;
	}

	bool Pin::IsSplit() const
	{
		return mIsSplit;
	}

	void Pin::AddConnectedPin(const PinID aPinID)
	{
		mConnectedPinIDs.push_back(aPinID);
	}

	void Pin::RemoveConnectedPin(const PinID aPinID)
	{
		std::erase(mConnectedPinIDs, aPinID);
	}
}