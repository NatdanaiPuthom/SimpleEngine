#pragma once
#include "../ScriptDefines.h"
#include "../Pin/Pin.h"
#include "PinView.h"

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