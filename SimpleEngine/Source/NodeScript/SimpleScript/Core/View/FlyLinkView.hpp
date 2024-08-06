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

		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] PinView GetInputPin() const;
		[[nodiscard]] PinView GetOutputPin() const;
		[[nodiscard]] LinkID GetID() const;

		explicit operator bool() const;

	private:

		const Link& GetLink() const;

	private:

		LinkID mLinkID = InvalidID<LinkID>();
		NodeGraphVariant mNodeGraphVariant;
	};
}