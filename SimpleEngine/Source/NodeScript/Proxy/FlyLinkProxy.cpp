#include "FlyLinkProxy.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "FlyNodeGraphProxy.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	LinkProxy::LinkProxy(const LinkID aLinkID, const NodeGraphProxy& aNodeGraph)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraph.GetVariant())
	{
	}

	LinkProxy::LinkProxy(const LinkID aLinkID, const NodeGraphVariantHandle& aNodeGraphVariant)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraphVariant)
	{
	}

	bool LinkProxy::IsDestroyed() const
	{
		return GetLink().mIsDestroyed;
	}

	PinProxy LinkProxy::GetInputPin() const
	{
		return PinProxy(GetLink().mInputPinID, NodeGraphProxy(mNodeGraphVariant));
	}

	PinProxy LinkProxy::GetOutputPin() const
	{
		return PinProxy(GetLink().mOutputPinID, NodeGraphProxy(mNodeGraphVariant));
	}

	LinkID LinkProxy::GetID() const
	{
		return mLinkID;
	}

	void LinkProxy::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLink(Internal::GetNodeGraph(mNodeGraphVariant), GetID(), aCommandTracker);
	}

	LinkProxy::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}


	bool operator==(const LinkProxy& a, const LinkProxy& b)
	{
		return a.mLinkID == b.mLinkID && a.mNodeGraphVariant == b.mNodeGraphVariant;
	}

	const Link& LinkProxy::GetLink() const
	{
		return NodeGraphProxy(mNodeGraphVariant).GetNodeGraph().mLinks.at(mLinkID);
	}
}
