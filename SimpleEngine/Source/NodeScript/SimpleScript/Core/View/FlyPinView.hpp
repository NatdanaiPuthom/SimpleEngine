#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinType.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class NodeGraphView;

	class PinView final
	{
	public:

		PinView() = default;
		PinView(PinID aPinID, const NodeGraphView& aNodeGraphView);

		[[nodiscard]] const std::vector<PinID>& GetConnectedPinIDs() const;
		[[nodiscard]] DataTypeID GetDataTypeID() const;
		[[nodiscard]] const std::string& GetPinTypeName() const;
		[[nodiscard]] eFlowType GetFlowType() const;
		[[nodiscard]] NodeID GetNodeID() const;

		[[nodiscard]] PinID GetID() const;

		[[nodiscard]] bool IsParentNodeReplacable() const;
		[[nodiscard]] bool HasAnyConnectedLinks() const;

		void DestroyConnectedLinks(CommandTracker* aCommandTracker);
		void Edit(CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const PinView& a, const PinView& b);
		
	private:

		[[nodiscard]] const Pin& GetPin() const;
		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinID mPinID = InvalidID<PinID>();
		NodeGraphVariant mNodeGraphVariant;
	};
}