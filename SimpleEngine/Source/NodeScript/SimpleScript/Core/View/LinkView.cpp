#include "LinkView.hpp"
#include "../ScriptNodeGraph.hpp"

namespace FLY_NAMESPACE
{



	LinkView::LinkView(LinkID aLinkID, const NodeGraph& aNodeGraph)
		: myLinkID(aLinkID)
		, myNodeGraph(&aNodeGraph)
	{
	}

	bool LinkView::IsDestroyed() const
	{
		return GetLink().mIsDestroyed;
	}

	PinView LinkView::GetInputPin() const
	{
		return PinView(GetLink().mInputPinID, *myNodeGraph);
	}

	PinView LinkView::GetOutputPin() const
	{
		return PinView(GetLink().mOutputPinID, *myNodeGraph);
	}

	LinkID LinkView::GetID() const
	{
		return myLinkID;
	}

	const Link& LinkView::GetLink() const
	{
		return myNodeGraph->mLinks.at(myLinkID);
	}
}
