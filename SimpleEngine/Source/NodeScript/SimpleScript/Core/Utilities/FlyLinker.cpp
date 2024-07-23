#include "FlyLinker.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "FlyProxy.hpp"

namespace FLY_NAMESPACE
{

	PinID ScriptLinker::GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType)
	{
		const Node& node = aNodeGraph.mNodes.at(aNodeID);
		switch (aPinFlowType)
		{
		case eFlowType::Input:
			if (aPinIndex < node.mInputPins.size())
			{
				return node.mInputPins.at(aPinIndex);
			}
			break;
		case eFlowType::Output:
			if (aPinIndex < node.mOutputPins.size())
			{
				return node.mOutputPins.at(aPinIndex);
			}
			break;
		default:
			break;
		}
		return InvalidID<PinID>();
	}

	size_t ScriptLinker::GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID)
	{
		const Pin& pin = aNodeGraph.mPins.at(aPinID);
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
		const Node& node = aNodeGraph.mNodes.at(pin.mNodeID);

		const std::vector<PinID>& pinIDs = pinType.mFlowType == eFlowType::Output ? node.mOutputPins : node.mInputPins;

		for (size_t i = 0; i < pinIDs.size(); ++i)
		{
			if (pinIDs[i] == aPinID)
			{
				return i;
			}
		}
		return InvalidID<size_t>();
	}

	PinID ScriptLinker::GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID)
	{
		const size_t pinIndex = GetPinIndex(aPreviousNodeGraph, aPreviousPinID);
		const Pin& pin = aPreviousNodeGraph.mPins.at(aPreviousPinID);
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
		return GetPinID(aNewNodeGraph, aNodeID, pinIndex, pinType.mFlowType);
	}

	static bool ArePinsLinkableByDataType(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2)
	{
		const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
		const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);
		return  Global::GetPinTypeManager().GetPinType(pin1.mTypeID).mDataTypeID == Global::GetPinTypeManager().GetPinType(pin2.mTypeID).mDataTypeID;
	}

	Link ScriptLinker::ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2)
	{
		const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
		const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);
		const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.mTypeID);
		const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.mTypeID);

		Link outputLink;

		switch (pinType1.mFlowType)
		{
		case eFlowType::Input:
			if (pinType2.mFlowType == eFlowType::Output)
			{
				if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
				{
					outputLink.mInputPinID = aPinID1;
					outputLink.mOutputPinID = aPinID2;
				}
			}
			break;
		case eFlowType::Output:
			if (pinType2.mFlowType == eFlowType::Input)
			{
				if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
				{
					outputLink.mInputPinID = aPinID2;
					outputLink.mOutputPinID = aPinID1;
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
		for (LinkID id = 0; id < aNodeGraph.mLinks.size(); ++id)
		{
			const Link& link = aNodeGraph.mLinks[id];
			if (!aIncludeDestroyed && link.mIsDestroyed)
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

	std::vector<LinkID> ScriptLinker::GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed)
	{
		std::vector<LinkID> linkIDs;
		const Pin& pin = ScriptProxy::GetPin(aNodeGraph, aPinID);

		for (PinID connectedPinID : pin.mConnectedPinIDs)
		{
			LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID, aIncludeDestroyed);
			assert(linkID != InvalidID<LinkID>());

			linkIDs.push_back(linkID);
		}

		return linkIDs;
	}

	std::vector<LinkID> ScriptLinker::GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID)
	{
		std::vector<LinkID> linkIDs;
		const Node& node = ScriptProxy::GetNode(aNodeGraph, aNodeID);

		for (PinID inputPinID : node.mInputPins)
		{
			std::vector<LinkID> inputLinks = GetLinkIDsByPin(aNodeGraph, inputPinID);
			linkIDs.insert(linkIDs.end(), inputLinks.begin(), inputLinks.end());
		}
		for (PinID mOutputPinID : node.mOutputPins)
		{
			std::vector<LinkID> outputLinks = GetLinkIDsByPin(aNodeGraph, mOutputPinID);
			linkIDs.insert(linkIDs.end(), outputLinks.begin(), outputLinks.end());
		}


		return linkIDs;
	}
}
