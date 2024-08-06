#include "FlyPinView.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "FlyNodeGraphView.hpp"
#include "Fly.hpp"

namespace FLY_NAMESPACE
{

	PinView::PinView(const PinID aPinID, const NodeGraphView& aNodeGraphView)
		: mPinID(aPinID)
		, mNodeGraphVariant(aNodeGraphView.GetVariant())
	{
	}

	const std::vector<PinID>& PinView::GetConnectedPinIDs() const
	{
		return GetPin().mConnectedPinIDs;
	}

	DataTypeID PinView::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}

	const std::string& PinView::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.mName == TypeIdentifierStr)
		{
			return Global::GetDataTypeManager().GetName(pinType.mDataTypeID);
		}
		return pinType.mName;
	}

	eFlowType PinView::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	NodeID PinView::GetNodeID() const
	{
		return GetPin().mNodeID;
	}

	PinID PinView::GetID() const
	{
		return mPinID;
	}

	bool PinView::IsParentNodeReplacable() const
	{
		return IsPinReplacable(*this, NodeGraphView(mNodeGraphVariant));
	}

	bool PinView::HasAnyConnectedLinks() const
	{
		return HasPinAnyConnectedLinks(*this);
	}

	void PinView::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		DestroyLinksByPin(*this, NodeGraphView(mNodeGraphVariant), aCommandTracker);
	}

	void PinView::Edit(CommandTracker* const aCommandTracker)
	{
		EditPin(*this, NodeGraphView(mNodeGraphVariant), aCommandTracker);
	}

	PinView::operator bool() const
	{
		return mPinID != InvalidID<PinID>();
	}

	const Pin& PinView::GetPin() const
	{
		return NodeGraphView(mNodeGraphVariant).GetNodeGraph().mPins.at(mPinID);
	}

	const PinType& PinView::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Global::GetPinTypeManager().GetPinType(pin.mTypeID);
	}

	bool operator==(const PinView& a, const PinView& b)
	{
		return NodeGraphView(a.mNodeGraphVariant) == NodeGraphView(b.mNodeGraphVariant) && a.mPinID == b.mPinID;
	}
}
