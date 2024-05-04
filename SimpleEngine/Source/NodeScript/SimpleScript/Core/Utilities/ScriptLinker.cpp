#include "ScriptLinker.h"
#include "../Script.h"
#include "../Node/NodeTypeRegistry.h"
#include "../Node/NodeManager.h"
#include "../Pin/PinTypeManager.h"
#include "ScriptProxy.h"

namespace SCR
{

	PinID ScriptLinker::GetPinID(const Script& aScript, const NodeID aNodeID, const size_t aPinIndex, const ePinFlowType aPinFlowType)
	{
		const Node& node = ScriptProxy::GetNode(aScript, aNodeID);
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

	size_t ScriptLinker::GetPinIndex(const Script& aScript, const PinID aPinID, const ePinFlowType aPinFlowType)
	{
		const NodeID nodeID = ScriptProxy::GetPin(aScript, aPinID).nodeID;
		const Node& node = ScriptProxy::GetNode(aScript, nodeID);

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

	bool ScriptLinker::AreDataPinsLinkable(const Script& aScript, const PinID aOutputPinID, const PinID aInputPinID)
	{
		const Pin& outputPin = ScriptProxy::GetPin(aScript, aOutputPinID);
		const Pin& inputPin = ScriptProxy::GetPin(aScript, aInputPinID);
		return PinTypeManager::GetPinType(outputPin.typeID).dataTypeID == PinTypeManager::GetPinType(inputPin.typeID).dataTypeID;
	}

	Link ScriptLinker::ArePinsLinkable(const Script& aScript, PinID aOutputPinID, PinID aInputPinID)
	{
		const Pin& outputPin = ScriptProxy::GetPin(aScript, aOutputPinID);
		const Pin& inputPin = ScriptProxy::GetPin(aScript, aInputPinID);

		switch (PinTypeManager::GetPinType(outputPin.typeID).flowType)
		{
		case ePinFlowType::Input:
			if (PinTypeManager::GetPinType(inputPin.typeID).flowType == ePinFlowType::Output)
			{
				if (AreDataPinsLinkable(aScript, aInputPinID, aOutputPinID))
				{
					std::swap(aOutputPinID, aInputPinID);
					return { aInputPinID, aOutputPinID };
				}
			}
			break;
		case ePinFlowType::Output:
			if (PinTypeManager::GetPinType(inputPin.typeID).flowType == ePinFlowType::Input)
			{
				bool linkable = AreDataPinsLinkable(aScript, aOutputPinID, aInputPinID);
				return linkable ? Link{ aInputPinID, aOutputPinID } : Link{};
			}
			break;
		default:
			break;
		}
		return Link{};
	}

	std::vector<Link> ScriptLinker::GetNodeLinks(const Script& aScript, const NodeID aNodeID)
	{
		const Node& node = ScriptProxy::GetNode(aScript, aNodeID);

		std::vector<Link> links;
		for (PinID inputPinID : node.inputPins)
		{
			const Pin& inputPin = ScriptProxy::GetPin(aScript, inputPinID);
			for (PinID connectedOutputPinID : inputPin.connectedPinIDs)
			{
				links.emplace_back(inputPinID, connectedOutputPinID);
			}
		}
		for (PinID outputPinID : node.outputPins)
		{
			const Pin& outputPin = ScriptProxy::GetPin(aScript, outputPinID);

			for (PinID connectedInputPinID : outputPin.connectedPinIDs)
			{
				links.emplace_back(connectedInputPinID, outputPinID);
			}
		}
		return links;
	}
}
