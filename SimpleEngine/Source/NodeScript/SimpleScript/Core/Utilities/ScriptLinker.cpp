#include "ScriptLinker.h"
#include "../Script.h"
#include "../Node/NodeTypeRegistry.h"
#include "../Node/NodeManager.h"
#include "../Pin/PinTypeManager.h"
#include "ScriptProxy.h"

namespace SCR
{

	PinID ScriptLinker::GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const ePinFlowType aPinFlowType)
	{
		const Node& node = ScriptProxy::GetNode(aNodeGraph, aNodeID);
		switch (aPinFlowType)
		{
		case ePinFlowType::Input:
			if (aPinIndex < node.inputPins.size())
			{
				return node.inputPins.at(aPinIndex);
			}
			break;
		case ePinFlowType::Output:
			if (aPinIndex < node.outputPins.size())
			{
				return node.outputPins.at(aPinIndex);
			}
			break;
		default:
			break;
		}
		return InvalidID<PinID>();
	}

	size_t ScriptLinker::GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID, const ePinFlowType aPinFlowType)
	{
		const NodeID nodeID = ScriptProxy::GetPin(aNodeGraph, aPinID).nodeID;
		const Node& node = ScriptProxy::GetNode(aNodeGraph, nodeID);

		const std::vector<PinID>& pinIDs = aPinFlowType == ePinFlowType::Output ? node.outputPins : node.inputPins;

		for (size_t i = 0; i < pinIDs.size(); ++i)
		{
			if (pinIDs[i] == aPinID)
			{
				return i;
			}
		}
		return InvalidID<size_t>();
	}

	static bool ArePinsLinkableByDataType(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2)
	{
		const Pin& pin1 = ScriptProxy::GetPin(aNodeGraph, aPinID1);
		const Pin& pin2 = ScriptProxy::GetPin(aNodeGraph, aPinID2);
		return PinTypeManager::GetPinType(pin1.typeID).dataTypeID == PinTypeManager::GetPinType(pin2.typeID).dataTypeID;
	}

	Link ScriptLinker::ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2)
	{
		const Pin& pin1 = ScriptProxy::GetPin(aNodeGraph, aPinID1);
		const Pin& pin2 = ScriptProxy::GetPin(aNodeGraph, aPinID2);
		const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.typeID);
		const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.typeID);

		Link outputLink;

		switch (pinType1.flowType)
		{
		case ePinFlowType::Input:
			if (pinType2.flowType == ePinFlowType::Output)
			{
				if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
				{
					outputLink.inputPinID = aPinID1;
					outputLink.outputPinID = aPinID2;
				}
			}
			break;
		case ePinFlowType::Output:
			if (pinType2.flowType == ePinFlowType::Input)
			{
				if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
				{
					outputLink.inputPinID = aPinID2;
					outputLink.outputPinID = aPinID1;
				}
			}
			break;
		default:
			break;
		}
		return outputLink;
	}

	LinkID ScriptLinker::GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed)
	{
		for (LinkID id = 0; id < aNodeGraph.myLinks.size(); ++id)
		{
			const Link& link = aNodeGraph.myLinks[id];
			if (!aIncludeDestroyed && link.isDestroyed)
			{
				continue;
			}
			if (link == Link{ aPinID1, aPinID2 } || link == Link{ aPinID2, aPinID1 })
			{
				return id;
			}
		}
		return InvalidID<LinkID>();
	}

	std::vector<LinkID> ScriptLinker::GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID)
	{
		std::vector<LinkID> linkIDs;
		const Pin& pin = ScriptProxy::GetPin(aNodeGraph, aPinID);

		for (PinID connectedPinID : pin.connectedPinIDs)
		{
			LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID);
			assert(linkID != InvalidID<LinkID>());

			linkIDs.push_back(linkID);
		}

		return linkIDs;
	}

	std::vector<LinkID> ScriptLinker::GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID)
	{
		std::vector<LinkID> linkIDs;
		const Node& node = ScriptProxy::GetNode(aNodeGraph, aNodeID);

		for (PinID inputPinID : node.inputPins)
		{
			std::vector<LinkID> inputLinks = GetLinkIDsByPin(aNodeGraph, inputPinID);
			linkIDs.insert(linkIDs.end(), inputLinks.begin(), inputLinks.end());
		}
		for (PinID outputPinID : node.outputPins)
		{
			std::vector<LinkID> outputLinks = GetLinkIDsByPin(aNodeGraph, outputPinID);
			linkIDs.insert(linkIDs.end(), outputLinks.begin(), outputLinks.end());
		}


		return linkIDs;
	}
}
