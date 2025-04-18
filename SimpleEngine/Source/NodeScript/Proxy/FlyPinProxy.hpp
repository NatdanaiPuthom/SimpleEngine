#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Pin/FlyIODirection.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class NodeGraphProxy;
	class Pin;
	class PinType;

	class PinProxy final
	{
	public:

		PinProxy() = default;
		PinProxy(PinID aPinID, const NodeGraphProxy& aNodeGraphProxy);
		PinProxy(PinID aPinID, const NodeGraphVariantHandle& aNodeGraphVariantHandle);

		[[nodiscard]] const std::vector<PinID>& GetConnectedPinIDs() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] std::string_view GetPinTypeName() const;
		[[nodiscard]] eIODirection GetIODirection() const;
		[[nodiscard]] NodeID GetNodeID() const;
		[[nodiscard]] PinTypeID GetPinTypeID() const;

		[[nodiscard]] PinID GetID() const;

		[[nodiscard]] bool IsParentNodeReplacable() const;
		[[nodiscard]] bool HasAnyConnectedLinks() const;
		[[nodiscard]] bool IsViewAndEditable() const;
		[[nodiscard]] bool IsViewable() const;
		[[nodiscard]] bool IsSplitable() const;
		[[nodiscard]] bool IsRecombinable() const;
		[[nodiscard]] std::vector<PinProxy> GetPotentialConnections() const;

		[[nodiscard]] NodeGraphVariantHandle GetNodeGraphVariant() const;

		void DestroyConnectedLinks(CommandTracker* aCommandTracker);
		void ViewAndEdit(CommandTracker* aCommandTracker);
		void View() const;
		void Split(CommandTracker* aCommandTracker);
		void RecombineParentPin(CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const PinProxy& a, const PinProxy& b);
		
	private:

		[[nodiscard]] const Pin& GetPin() const;
		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinID mPinID;
		NodeGraphVariantHandle mNodeGraphVariant;
	};
}