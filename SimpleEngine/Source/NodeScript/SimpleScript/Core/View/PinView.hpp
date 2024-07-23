#pragma once
#include "../ScriptDefines.hpp"
#include "../Pin/Pin.hpp"
#include "../Pin/PinType.hpp"

namespace FLY_NAMESPACE
{

	class PinView final
	{

	public:

		PinView() = default;
		PinView(PinID aPinID, const NodeGraph& aNodeGraph);

		const std::vector<PinID>& GetConnectedPinIDs() const;
		DataTypeID GetDataTypeID() const;
		const std::string& GetPinTypeName() const;
		eFlowType GetFlowType() const;
		NodeID GetNodeID() const;

		PinID GetID() const;

		explicit operator bool() const;
		
	private:

		const Pin& GetPin() const;
		const PinType& GetPinType() const;

	private:

		PinID mPinID = InvalidID<PinID>();
		const NodeGraph* mNodeGraph;
	};
}