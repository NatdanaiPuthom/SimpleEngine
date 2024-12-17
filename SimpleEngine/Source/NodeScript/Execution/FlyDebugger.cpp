#include "FlyDebugger.hpp"

namespace FLY_NAMESPACE
{

	void Debugger::AddTraversedLink(LinkID aLinkID, const NodeGraphVariantHandle& aNodeGraph)
	{
		mTraversedLinks.push_back(LinkRef{ .mNodeGraphVariantHandle = aNodeGraph, .mLinkID = aLinkID });
	}

	void Debugger::ClearTraversedLinks()
	{
		mTraversedLinks.clear();
	}
}
