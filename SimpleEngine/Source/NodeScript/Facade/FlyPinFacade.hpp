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
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] std::string_view GetPinTypeName() const;
		[[nodiscard]] eFlowType GetFlowType() const;
		[[nodiscard]] NodeID GetNodeID() const;
		[[nodiscard]] PinTypeID GetPinTypeID() const;

		[[nodiscard]] PinID GetID() const;

		[[nodiscard]] bool IsParentNodeReplacable() const;
		[[nodiscard]] bool HasAnyConnectedLinks() const;
		[[nodiscard]] bool IsViewAndEditable() const;
		[[nodiscard]] bool IsViewable() const;
		[[nodiscard]] bool IsSplitable() const;
		[[nodiscard]] bool IsRecombinable() const;
		[[nodiscard]] std::vector<PinFacade> GetPotentialConnections() const;
		//[[nodiscard]] std::vector<PinFacade> GetSplitPins() const;

		[[nodiscard]] NodeGraphVariantHandle GetNodeGraphVariant() const;

		void DestroyConnectedLinks(CommandTracker* aCommandTracker);
		void ViewAndEdit(CommandTracker* aCommandTracker);
		void View() const;
		void Split(CommandTracker* aCommandTracker);
		void RecombineParentPin(CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const PinFacade& a, const PinFacade& b);
		
	private:

		[[nodiscard]] const Pin& GetPin() const;
		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinID mPinID;
		NodeGraphVariantHandle mNodeGraphVariant;
	};
}