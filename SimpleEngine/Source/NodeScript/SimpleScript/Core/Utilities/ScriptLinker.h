#pragma once
#include "../ScriptDefines.h"
#include "../Pin/Pin.h"
#include "../Pin/PinType.h"

namespace SCR
{

	class Script;
	class NodeGraph;

	class ScriptLinker
	{
	public:

		static PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const ePinFlowType aPinFlowType);
		static size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID, const ePinFlowType aPinFlowType);

		static bool AreDataPinsLinkable(const NodeGraph& aNodeGraph, const PinID anOutputPinID, const PinID anInputPinID);

		static Link ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2);

		static std::vector<Link> GetNodeLinks(const NodeGraph& aNodeGraph, const NodeID aNodeID);
	};
}