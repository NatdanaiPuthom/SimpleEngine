#include "LinkView.h"
#include "../ScriptNodeGraph.h"

namespace SCR
{



	LinkView::LinkView(LinkID aLinkID, const NodeGraph& aNodeGraph)
		: myLinkID(aLinkID)
		, myNodeGraph(&aNodeGraph)
	{
	}

	bool LinkView::IsDestroyed() const
	{
		return GetLink().isDestroyed;
	}

	PinView LinkView::GetInputPin() const
	{
		return PinView(GetLink().inputPinID, *myNodeGraph);
	}

	PinView LinkView::GetOutputPin() const
	{
		return PinView(GetLink().outputPinID, *myNodeGraph);
	}

	LinkID LinkView::GetID() const
	{
		return myLinkID;
	}

	const Link& LinkView::GetLink() const
	{
		return myNodeGraph->myLinks.at(myLinkID);
	}
}
