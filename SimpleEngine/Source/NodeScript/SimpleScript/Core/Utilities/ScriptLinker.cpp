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

	bool ScriptLinker::AreDataPinsLinkable(const NodeGraph& aNodeGraph, const PinID aOutputPinID, const PinID aInputPinID)
	{
		const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, aOutputPinID);
		const Pin& inputPin = ScriptProxy::GetPin(aNodeGraph, aInputPinID);
		return PinTypeManager::GetPinType(outputPin.typeID).dataTypeID == PinTypeManager::GetPinType(inputPin.typeID).dataTypeID;
	}

	Link ScriptLinker::ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aOutputPinID, PinID aInputPinID)
	{
		const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, aOutputPinID);
		const Pin& inputPin = ScriptProxy::GetPin(aNodeGraph, aInputPinID);

		switch (PinTypeManager::GetPinType(outputPin.typeID).flowType)
		{
		case ePinFlowType::Input:
			if (PinTypeManager::GetPinType(inputPin.typeID).flowType == ePinFlowType::Output)
			{
				if (AreDataPinsLinkable(aNodeGraph, aInputPinID, aOutputPinID))
				{
					std::swap(aOutputPinID, aInputPinID);
					return { aInputPinID, aOutputPinID };
				}
			}
			break;
		case ePinFlowType::Output:
			if (PinTypeManager::GetPinType(inputPin.typeID).flowType == ePinFlowType::Input)
			{
				bool linkable = AreDataPinsLinkable(aNodeGraph, aOutputPinID, aInputPinID);
				return linkable ? Link{ aInputPinID, aOutputPinID } : Link{};
			}
			break;
		default:
			break;
		}
		return Link{};
	}

	std::vector<Link> ScriptLinker::GetNodeLinks(const NodeGraph& aNodeGraph, const NodeID aNodeID)
	{
		const Node& node = ScriptProxy::GetNode(aNodeGraph, aNodeID);

		std::vector<Link> links;
		for (PinID inputPinID : node.inputPins)
		{
			const Pin& inputPin = ScriptProxy::GetPin(aNodeGraph, inputPinID);
			for (PinID connectedOutputPinID : inputPin.connectedPinIDs)
			{
				links.emplace_back(inputPinID, connectedOutputPinID);
			}
		}
		for (PinID outputPinID : node.outputPins)
		{
			const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, outputPinID);

			for (PinID connectedInputPinID : outputPin.connectedPinIDs)
			{
				links.emplace_back(connectedInputPinID, outputPinID);
			}
		}
		return links;
	}
}
