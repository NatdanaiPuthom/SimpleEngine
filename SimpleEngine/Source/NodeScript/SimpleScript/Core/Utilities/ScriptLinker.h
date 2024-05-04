#pragma once
#include "../ScriptDefines.h"
#include "../Pin/Pin.h"
#include "../Pin/PinType.h"

namespace SCR
{

	class Script;

	class ScriptLinker
	{
	public:

		static PinID GetPinID(const Script& aScript, const NodeID aNodeID, const size_t aPinIndex, const ePinFlowType aPinFlowType);
		static size_t GetPinIndex(const Script& aScript, const PinID aPinID, const ePinFlowType aPinFlowType);

		static bool AreDataPinsLinkable(const Script& aScript, const PinID anOutputPinID, const PinID anInputPinID);

		static Link ArePinsLinkable(const Script& aScript, PinID aPinID1, PinID aPinID2);

		static std::vector<Link> GetNodeLinks(const Script& aScript, const NodeID aNodeID);
	};
}