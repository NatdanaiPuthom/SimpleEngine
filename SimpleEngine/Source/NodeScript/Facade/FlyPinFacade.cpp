#include "FlyPinFacade.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	PinFacade::PinFacade(const PinID aPinID, const NodeGraphFacade& aNodeGraphView)
		: mPinID(aPinID)
		, mNodeGraphVariant(aNodeGraphView.GetVariant())
	{
	}

	const std::vector<PinID>& PinFacade::GetConnectedPinIDs() const
	{
		return GetPin().mConnectedPinIDs;
	}

	DataTypeID PinFacade::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}

	const std::string& PinFacade::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.mName == TypeIdentifierStr)
		{
			return Global::GetDataTypeManager().GetName(pinType.mDataTypeID);
		}
		return pinType.mName;
	}

	eFlowType PinFacade::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	NodeID PinFacade::GetNodeID() const
	{
		return GetPin().mNodeID;
	}

	PinID PinFacade::GetID() const
	{
		return mPinID;
	}

	bool PinFacade::IsParentNodeReplacable() const
	{
		return Internal::IsNodeReplacable(NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), GetNodeID());
	}

	bool PinFacade::HasAnyConnectedLinks() const
	{
		return HasPinAnyConnectedLinks(*this);
	}

	void PinFacade::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinksByPin(NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), GetID(), aCommandTracker);
	}

	void PinFacade::Edit(CommandTracker* const aCommandTracker)
	{
		Internal::EditPin(GetID(), NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), aCommandTracker);
	}

	PinFacade::operator bool() const
	{
		return mPinID != InvalidID<PinID>();
	}

	const Pin& PinFacade::GetPin() const
	{
		return NodeGraphFacade(mNodeGraphVariant).GetNodeGraph().mPins.at(mPinID);
	}

	const PinType& PinFacade::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Global::GetPinTypeManager().GetPinType(pin.mTypeID);
	}

	bool operator==(const PinFacade& a, const PinFacade& b)
	{
		return NodeGraphFacade(a.mNodeGraphVariant) == NodeGraphFacade(b.mNodeGraphVariant) && a.mPinID == b.mPinID;
	}
}
