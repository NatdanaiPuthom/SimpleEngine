#include "FlyLink.hpp"

namespace FLY_NAMESPACE
{

	Link::Link(const PinID aInputPinID, const PinID aOutputPinID)
		: mInputPinID(aInputPinID)
		, mOutputPinID(aOutputPinID)
	{
	}

	PinID Link::GetInputPinID() const
	{
		return mInputPinID;
	}

	PinID Link::GetOutputPinID() const
	{
		return mOutputPinID;
	}

	void Link::SetIsDestroyed(bool aIsDestroyed)
	{
		mIsDestroyed = aIsDestroyed;
	}

	bool Link::IsDestroyed() const
	{
		return mIsDestroyed;
	}
}