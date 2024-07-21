#include "LinkView.hpp"
#include "../ScriptNodeGraph.hpp"

namespace FLY_NAMESPACE
{



	LinkView::LinkView(const LinkID aLinkID, const NodeGraph& aNodeGraph)
		: mLinkID(aLinkID)
		, mNodeGraph(&aNodeGraph)
	{
	}

	bool LinkView::IsDestroyed() const
	{
		return GetLink().mIsDestroyed;
	}

	PinView LinkView::GetInputPin() const
	{
		return PinView(GetLink().mInputPinID, *mNodeGraph);
	}

	PinView LinkView::GetOutputPin() const
	{
		return PinView(GetLink().mOutputPinID, *mNodeGraph);
	}

	LinkID LinkView::GetID() const
	{
		return mLinkID;
	}

	LinkView::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}

	const Link& LinkView::GetLink() const
	{
		return mNodeGraph->mLinks.at(mLinkID);
	}
}
