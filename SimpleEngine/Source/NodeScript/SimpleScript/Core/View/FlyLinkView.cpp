#include "FlyLinkView.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "FlyNodeGraphView.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{

	LinkView::LinkView(const LinkID aLinkID, const NodeGraphView& aNodeGraphView)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraphView.GetVariant())
	{
	}

	bool LinkView::IsDestroyed() const
	{
		return GetLink().mIsDestroyed;
	}

	PinView LinkView::GetInputPin() const
	{
		return PinView(GetLink().mInputPinID, NodeGraphView(mNodeGraphVariant));
	}

	PinView LinkView::GetOutputPin() const
	{
		return PinView(GetLink().mOutputPinID, NodeGraphView(mNodeGraphVariant));
	}

	LinkID LinkView::GetID() const
	{
		return mLinkID;
	}

	void LinkView::Destroy(CommandTracker* const aCommandTracker)
	{
		DestroyLink(*this, NodeGraphView(mNodeGraphVariant), aCommandTracker);
	}

	LinkView::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}

	const Link& LinkView::GetLink() const
	{
		return NodeGraphView(mNodeGraphVariant).GetNodeGraph().mLinks.at(mLinkID);
	}
}
