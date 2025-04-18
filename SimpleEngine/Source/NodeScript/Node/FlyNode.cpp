#include "FlyNode.hpp"

namespace FLY_NAMESPACE
{
	Node::Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins)
		: mTypeID(aTypeID)
		, mInputPins(aInputPins)
		, mOutputPins(aOutputPins)
		, mSplitInputPins(mInputPins)
		, mSplitOutputPins(mOutputPins)
	{
	}

	void Node::SetPosition(const Vec2 aPosition)
	{
		mPosition = aPosition;
	}

	Vec2 Node::GetPosition() const
	{
		return mPosition;
	}
	
	bool Node::IsDestroyed() const
	{
		return mIsDestroyed;
	}

	void Node::SetIsDestroyed(bool aIsDestroyed)
	{
		mIsDestroyed = aIsDestroyed;
	}
}