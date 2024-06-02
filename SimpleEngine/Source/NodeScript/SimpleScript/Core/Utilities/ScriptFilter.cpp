#include "ScriptFilter.h"
#include "../Script.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Pin/PinTypeManager.h"
#include "ScriptProxy.h"

namespace SCR
{

	ScriptFilter::~ScriptFilter()
	{
	}

	std::vector<PinID> ScriptFilter::GetInputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> inputPinIDs;

		for (PinID i = 0; i < ScriptProxy::GetPins(aNodeGraph).size(); i++)
		{
			if (PinTypeManager::GetPinType(ScriptProxy::GetPin(aNodeGraph, i).typeID).flowType == ePinFlowType::Input)
			{
				const NodeID nodeID = ScriptProxy::GetPin(aNodeGraph, i).nodeID;

				if (!ScriptProxy::GetNode(aNodeGraph, nodeID).isDestroyed)
				{
					inputPinIDs.push_back(i);
				}
			}
		}
		return inputPinIDs;
	}

	std::vector<PinID> ScriptFilter::GetOutputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> pinIDs;

		for (PinID i = 0; i < ScriptProxy::GetPins(aNodeGraph).size(); i++)
		{
			const Pin& pin = ScriptProxy::GetPin(aNodeGraph, i);
			if (PinTypeManager::GetPinType(pin.typeID).flowType == ePinFlowType::Output)
			{

				if (!ScriptProxy::GetNode(aNodeGraph, pin.nodeID).isDestroyed)
				{
					pinIDs.push_back(i);
				}
			}
		}
		return pinIDs;
	}

	std::vector<PinID> ScriptFilter::GetNonConnectedInputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> inputPinIDs = GetInputPins(aNodeGraph);

		return Stream<PinID>(inputPinIDs).Filter([&](PinID aPinID) -> bool
			{
				return ScriptProxy::GetPin(aNodeGraph, aPinID).connectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedOutputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> pinIDs = GetOutputPins(aNodeGraph);

		return Stream<PinID>(pinIDs).Filter([&](PinID aPinID) -> bool
			{
				return ScriptProxy::GetPin(aNodeGraph, aPinID).connectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfType(const NodeGraph& aNodeGraph, const ePinFlowType aFlowType)
	{
		switch (aFlowType)
		{
		case ePinFlowType::Input:
			return GetNonConnectedInputPins(aNodeGraph);
			break;
		case ePinFlowType::Output:
			return GetNonConnectedOutputPins(aNodeGraph);
			break;
		default:
			break;
		}
		return std::vector<PinID>();
	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfTypeAndHash(const NodeGraph& aNodeGraph, const ePinFlowType aFlowType, const DataTypeID aDataTypeID)
	{
		std::vector<PinID> pinIDs;
		std::vector<PinID> pinTypeFilter = GetNonConnectedPinsOfType(aNodeGraph, aFlowType);
		switch (aFlowType)
		{
		case ePinFlowType::Input:

			for (PinID id : pinTypeFilter)
			{
				if (PinTypeManager::GetPinType(ScriptProxy::GetPin(aNodeGraph, id).typeID).dataTypeID == aDataTypeID)
				{
					pinIDs.push_back(id);
				}
			}
			break;

		case ePinFlowType::Output:
			for (PinID id : pinTypeFilter)
			{
				if (PinTypeManager::GetPinType(ScriptProxy::GetPin(aNodeGraph, id).typeID).dataTypeID == aDataTypeID)
				{
					pinIDs.push_back(id);
				}
			}
			break;
		default:
			break;
		}

		return pinIDs;
	}
}
