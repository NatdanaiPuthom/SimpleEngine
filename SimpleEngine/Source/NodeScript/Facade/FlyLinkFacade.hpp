#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "FlyPinFacade.hpp"

namespace FLY_NAMESPACE
{

	class NodeGraph;
	class CommandTracker;

	class LinkFacade final
	{
	public:

		LinkFacade() = default;
		LinkFacade(LinkID aLinkID, const NodeGraphFacade& aNodeGraphFacade);

		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] PinFacade GetInputPin() const;
		[[nodiscard]] PinFacade GetOutputPin() const;
		[[nodiscard]] LinkID GetID() const;

		void Destroy(CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Link& GetLink() const;

	private:

		LinkID mLinkID = InvalidID<LinkID>();
		NodeGraphVariant mNodeGraphVariant;
	};
}