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

		const std::vector<PinID>& GetConnectedPinIDs() const;
		DataTypeID GetDataTypeID() const;
		const std::string& GetPinTypeName() const;
		eFlowType GetFlowType() const;
		NodeID GetNodeID() const;

		PinID GetID() const;

		void DestroyConnectedLinks(CommandTracker* aCommandTracker);

		explicit operator bool() const;
		
	private:

		const Pin& GetPin() const;
		const PinType& GetPinType() const;

	private:

		PinID mPinID = InvalidID<PinID>();
		NodeGraphVariant mNodeGraphVariant;
	};
}