#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinType.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class NodeGraphFacade;

	class PinFacade final
	{
	public:

		PinFacade() = default;
		PinFacade(PinID aPinID, const NodeGraphFacade& aNodeGraphFacade);

		[[nodiscard]] const std::vector<PinID>& GetConnectedPinIDs() const;
		[[nodiscard]] DataTypeID GetDataTypeID() const;
		[[nodiscard]] const std::string& GetPinTypeName() const;
		[[nodiscard]] eFlowType GetFlowType() const;
		[[nodiscard]] NodeID GetNodeID() const;

		[[nodiscard]] PinID GetID() const;

		[[nodiscard]] bool IsParentNodeReplacable() const;
		[[nodiscard]] bool HasAnyConnectedLinks() const;

		[[nodiscard]] NodeGraphVariantHandle GetNodeGraphVariant() const;

		void DestroyConnectedLinks(CommandTracker* aCommandTracker);
		void ViewAndEdit(CommandTracker* aCommandTracker);
		void View() const;

		explicit operator bool() const;

		friend bool operator==(const PinFacade& a, const PinFacade& b);
		
	private:

		[[nodiscard]] const Pin& GetPin() const;
		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinID mPinID = InvalidID<PinID>();
		NodeGraphVariantHandle mNodeGraphVariant;
	};
}