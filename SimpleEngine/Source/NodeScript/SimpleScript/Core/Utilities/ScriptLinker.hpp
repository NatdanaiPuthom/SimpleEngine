#pragma once
#include "../ScriptDefines.hpp"
#include "../Pin/Pin.hpp"
#include "../Pin/PinType.hpp"

namespace SCR
{

	class Class;
	class NodeGraph;

	class ScriptLinker
	{
	public:

		static PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType);
		static size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID);
		static PinID GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID);

		static Link ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2);

		static LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed = false);
		static std::vector<LinkID> GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed = false);
		static std::vector<LinkID> GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID);
	};
}