#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "FlyPinView.hpp"

namespace FLY_NAMESPACE
{

	class NodeGraph;

	class LinkView final
	{
	public:

		LinkView() = default;
		LinkView(LinkID aLinkID, const NodeGraphView& aNodeGraphView);

		bool IsDestroyed() const;
		PinView GetInputPin() const;
		PinView GetOutputPin() const;
		LinkID GetID() const;

		explicit operator bool() const;

	private:

		const Link& GetLink() const;

	private:

		LinkID mLinkID = InvalidID<LinkID>();
		NodeGraphVariant mNodeGraphVariant;
	};
}