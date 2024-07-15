#pragma once
#include "../ScriptDefines.h"
#include "../Pin/Pin.h"
#include "../Pin/PinType.h"

namespace SCR
{

	class PinView final
	{

	public:

		PinView(PinID aPinID, const NodeGraph& aNodeGraph);

		const std::vector<PinID>& GetConnectedPinIDs() const;
		DataTypeID GetDataTypeID() const;
		const std::string& GetPinTypeName() const;
		
	private:

		const Pin& GetPin() const;
		const PinType& GetPinType() const;

	private:

		PinID myPinID;
		const NodeGraph* myNodeGraph;
	};
}