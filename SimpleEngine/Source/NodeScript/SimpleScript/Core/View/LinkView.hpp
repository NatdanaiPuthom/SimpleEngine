#pragma once
#include "../ScriptDefines.hpp"
#include "../Pin/Pin.hpp"
#include "PinView.hpp"

namespace FLY_NAMESPACE
{

	class NodeGraph;

	class LinkView final
	{
	public:

		LinkView() = default;
		LinkView(LinkID aLinkID, const NodeGraph& aNodeGraph);

		bool IsDestroyed() const;
		PinView GetInputPin() const;
		PinView GetOutputPin() const;
		LinkID GetID() const;

		explicit operator bool() const;

	private:

		const Link& GetLink() const;

	private:

		LinkID mLinkID = InvalidID<LinkID>();
		const NodeGraph* mNodeGraph = nullptr;
	};
}