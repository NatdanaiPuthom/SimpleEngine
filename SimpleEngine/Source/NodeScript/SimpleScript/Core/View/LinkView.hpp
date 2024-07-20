#pragma once
#include "../ScriptDefines.hpp"
#include "../Pin/Pin.hpp"
#include "PinView.hpp"

namespace SCR
{

	class NodeGraph;

	class LinkView final
	{
	public:

		LinkView(LinkID aLinkID, const NodeGraph& aNodeGraph);

		bool IsDestroyed() const;
		PinView GetInputPin() const;
		PinView GetOutputPin() const;
		LinkID GetID() const;

	private:

		const Link& GetLink() const;

	private:

		LinkID myLinkID;
		const NodeGraph* myNodeGraph;
	};
}