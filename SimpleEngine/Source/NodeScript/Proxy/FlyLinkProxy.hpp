#pragma once
#include "../FlyDefines.hpp"
#include "FlyPinProxy.hpp"

namespace FLY_NAMESPACE
{
	class CommandTracker;
	class Link;

	class LinkProxy final
	{
	public:

		LinkProxy() = default;
		LinkProxy(LinkID aLinkID, const NodeGraphProxy& aNodeGraph);
		LinkProxy(LinkID aLinkID, const NodeGraphVariantHandle& aNodeGraphVariant);

		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] PinProxy GetInputPin() const;
		[[nodiscard]] PinProxy GetOutputPin() const;
		[[nodiscard]] LinkID GetID() const;

		void Destroy(CommandTracker* aCommandTracker);

		[[nodiscard]] explicit operator bool() const;

		[[nodiscard]] friend bool operator==(const LinkProxy& a, const LinkProxy& b);

	private:

		const Link& GetLink() const;

	private:

		LinkID mLinkID = InvalidID<LinkID>();
		NodeGraphVariantHandle mNodeGraphVariant;
	};
}