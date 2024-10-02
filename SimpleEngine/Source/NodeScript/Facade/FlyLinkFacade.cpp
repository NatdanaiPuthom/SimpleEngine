#include "FlyLinkFacade.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{

	LinkFacade::LinkFacade(const LinkID aLinkID, const NodeGraphFacade& aNodeGraphFacade)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraphFacade.GetVariant())
	{
	}

	bool LinkFacade::IsDestroyed() const
	{
		return GetLink().mIsDestroyed;
	}

	PinFacade LinkFacade::GetInputPin() const
	{
		return PinFacade(GetLink().mInputPinID, NodeGraphFacade(mNodeGraphVariant));
	}

	PinFacade LinkFacade::GetOutputPin() const
	{
		return PinFacade(GetLink().mOutputPinID, NodeGraphFacade(mNodeGraphVariant));
	}

	LinkID LinkFacade::GetID() const
	{
		return mLinkID;
	}

	void LinkFacade::Destroy(CommandTracker* const aCommandTracker)
	{
		DestroyLink(*this, NodeGraphFacade(mNodeGraphVariant), aCommandTracker);
	}

	LinkFacade::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}

	const Link& LinkFacade::GetLink() const
	{
		return NodeGraphFacade(mNodeGraphVariant).GetNodeGraph().mLinks.at(mLinkID);
	}
}
