#include "ScriptFilter.hpp"
#include "../Utilities/ScriptUtilities.hpp"
#include "../Pin/PinTypeManager.hpp"
#include "ScriptProxy.hpp"
#include "../Node/Node.hpp"
#include "Global/ScriptGlobal.hpp"

namespace FLY_NAMESPACE
{

	ScriptFilter::~ScriptFilter()
	{
	}

	std::vector<PinID> ScriptFilter::GetInputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> inputPinIDs;

		for (PinID i = 0; i < ScriptProxy::GetPins(aNodeGraph).size(); i++)
		{
			if (Global::GetPinTypeManager().GetPinType(ScriptProxy::GetPin(aNodeGraph, i).mTypeID).mFlowType == eFlowType::Input)
			{
				const NodeID mNodeID = ScriptProxy::GetPin(aNodeGraph, i).mNodeID;

				if (!ScriptProxy::GetNode(aNodeGraph, mNodeID).mIsDestroyed)
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
			if (Global::GetPinTypeManager().GetPinType(pin.mTypeID).mFlowType == eFlowType::Output)
			{

				if (!ScriptProxy::GetNode(aNodeGraph, pin.mNodeID).mIsDestroyed)
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
				return ScriptProxy::GetPin(aNodeGraph, aPinID).mConnectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedOutputPins(const NodeGraph& aNodeGraph)
	{
		std::vector<PinID> pinIDs = GetOutputPins(aNodeGraph);

		return Stream<PinID>(pinIDs).Filter([&](PinID aPinID) -> bool
			{
				return ScriptProxy::GetPin(aNodeGraph, aPinID).mConnectedPinIDs.empty();
			})
			.GetCopy();

	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfType(const NodeGraph& aNodeGraph, const eFlowType aFlowType)
	{
		switch (aFlowType)
		{
		case eFlowType::Input:
			return GetNonConnectedInputPins(aNodeGraph);
			break;
		case eFlowType::Output:
			return GetNonConnectedOutputPins(aNodeGraph);
			break;
		default:
			break;
		}
		return std::vector<PinID>();
	}

	std::vector<PinID> ScriptFilter::GetNonConnectedPinsOfTypeAndHash(const NodeGraph& aNodeGraph, const eFlowType aFlowType, const DataTypeID aDataTypeID)
	{
		std::vector<PinID> pinIDs;
		std::vector<PinID> pinTypeFilter = GetNonConnectedPinsOfType(aNodeGraph, aFlowType);
		switch (aFlowType)
		{
		case eFlowType::Input:

			for (PinID id : pinTypeFilter)
			{
				if (Global::GetPinTypeManager().GetPinType(ScriptProxy::GetPin(aNodeGraph, id).mTypeID).mDataTypeID == aDataTypeID)
				{
					pinIDs.push_back(id);
				}
			}
			break;

		case eFlowType::Output:
			for (PinID id : pinTypeFilter)
			{
				if (Global::GetPinTypeManager().GetPinType(ScriptProxy::GetPin(aNodeGraph, id).mTypeID).mDataTypeID == aDataTypeID)
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
