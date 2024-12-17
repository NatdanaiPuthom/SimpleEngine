#include "FlyLinkFacade.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

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
		Internal::DestroyLink(NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), GetID(), aCommandTracker);
	}

	LinkFacade::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}


	bool operator==(const LinkFacade& a, const LinkFacade& b)
	{
		return a.mLinkID == b.mLinkID && a.mNodeGraphVariant == b.mNodeGraphVariant;
	}

	const Link& LinkFacade::GetLink() const
	{
		return NodeGraphFacade(mNodeGraphVariant).GetNodeGraph().mLinks.at(mLinkID);
	}
}
