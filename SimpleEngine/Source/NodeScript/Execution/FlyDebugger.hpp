#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"
#include "Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{
	struct NodeBreakpoint
	{
		NodeExecutionData mNodeExecutionData;

		explicit operator bool() const
		{
			return static_cast<bool>(mNodeExecutionData.mNodeRef);
		}
	};

	struct LinkRef
	{
		NodeGraphVariantHandle mNodeGraphVariantHandle;
		LinkID mLinkID = InvalidID<LinkID>();
	};

	class Debugger final
	{
	public:
		Debugger() = default;



		void AddBreakpoint(const NodeRef& aNodeRef)
		{
			mBreakpoint.mNodeExecutionData.mNodeRef = aNodeRef;
		}

		void AddTraversedLink(LinkID aLinkID, const NodeGraphVariantHandle& aNodeGraph);

		void ClearTraversedLinks();

		const std::vector<LinkRef>& GetTraversedLinks() const
		{
			return mTraversedLinks;
		}

	private:

		NodeBreakpoint mBreakpoint;
		std::vector<LinkRef> mTraversedLinks;
	};
}