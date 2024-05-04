#include "ScriptFilter.h"
#include "../Script.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Pin/PinTypeManager.h"

namespace SCR
{

	ScriptFilter::ScriptFilter(const Script& aScript)
		: myScript(aScript)
	{
	}

	ScriptFilter::~ScriptFilter()
	{
	}

	std::vector<PinID> ScriptFilter::GetInputPins() const
	{
		std::vector<PinID> inputPinIDs;

		for (PinID i = 0; i < ScriptProxy::GetPins(myScript).size(); i++)
		{
			if (PinTypeManager::GetPinType(ScriptProxy::GetPin(myScript, i).typeID).flowType == ePinFlowType::Input)
			{
				const NodeID nodeID = ScriptProxy::GetPin(myScript, i).nodeID;

				if (!ScriptProxy::GetNode(myScript, nodeID).isDestroyed)
				{
					inputPinIDs.push_back(i);
				}
			}
		}
		return inputPinIDs;
	}

	std::vector<PinID> ScriptFilter::GetOutputPins() const
	{
		std::vector<PinID> pinIDs;

		for (PinID i = 0; i < ScriptProxy::GetPins(myScript).size(); i++)
		{
			const Pin& pin = ScriptProxy::GetPin(myScript, i);
			if (PinTypeManager::GetPinType(pin.typeID).flowType == ePinFlowType::Output)
			{

				if (!ScriptProxy::GetNode(myScript, pin.nodeID).isDestroyed)
				{
					pinIDs.push_back(i);
				}
			}
		}
		return pinIDs;
	}

	std::vector<PinID> ScriptFilter::GetNonConnectedInputPins() const
	{
		std::vector<PinID> inputPinIDs = GetInputPins();

		return Stream<PinID>(inputPinIDs).Filter([&](PinID aPinID) -> bool
			{
				return ScriptProxy::GetPin(myScript, aPinID).connectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedOutputPins() const
	{
		std::vector<PinID> pinIDs = GetOutputPins();

		return Stream<PinID>(pinIDs).Filter([&](PinID aPinID) -> bool
			{
				return ScriptProxy::GetPin(myScript, aPinID).connectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfType(const ePinFlowType aFlowType) const
	{
		switch (aFlowType)
		{
		case ePinFlowType::Input:
			return GetNonConnectedInputPins();
			break;
		case ePinFlowType::Output:
			return GetNonConnectedOutputPins();
			break;
		default:
			break;
		}
		return std::vector<PinID>();
	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfTypeAndHash(const ePinFlowType aFlowType, const DataTypeID aDataTypeID) const
	{
		std::vector<PinID> pinIDs;
		std::vector<PinID> pinTypeFilter = GetNonConnectedPinsOfType(aFlowType);
		switch (aFlowType)
		{
		case ePinFlowType::Input:

			for (PinID id : pinTypeFilter)
			{
				if (PinTypeManager::GetPinType(ScriptProxy::GetPin(myScript, id).typeID).dataTypeID == aDataTypeID)
				{
					pinIDs.push_back(id);
				}
			}
			break;

		case ePinFlowType::Output:
			for (PinID id : pinTypeFilter)
			{
				if (PinTypeManager::GetPinType(ScriptProxy::GetPin(myScript, id).typeID).dataTypeID == aDataTypeID)
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
