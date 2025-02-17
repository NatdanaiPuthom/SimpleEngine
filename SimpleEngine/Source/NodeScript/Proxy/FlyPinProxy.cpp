#include "FlyPinProxy.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	PinProxy::PinProxy(const PinID aPinID, const NodeGraphProxy& aNodeGraphProxy)
		: mPinID(aPinID)
		, mNodeGraphVariant(aNodeGraphProxy.GetVariant())
	{
	}

	PinProxy::PinProxy(const PinID aPinID, const NodeGraphVariantHandle& aNodeGraphVariant)
		: mPinID(aPinID)
		, mNodeGraphVariant(aNodeGraphVariant)
	{
	}

	const std::vector<PinID>& PinProxy::GetConnectedPinIDs() const
	{
		return GetPin().mConnectedPinIDs;
	}

	GenericDataTypeID PinProxy::GetDataTypeID() const
	{
		return GetPinType().mGenericDataTypeID;
	}

	std::string_view PinProxy::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.mName == TypeIdentifierStr)
		{
			return Internal::GetDataTypeManager().GetName(pinType.mGenericDataTypeID);
		}
		return pinType.mName;
	}

	eFlowType PinProxy::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	NodeID PinProxy::GetNodeID() const
	{
		return GetPin().mNodeID;
	}

	PinTypeID PinProxy::GetPinTypeID() const
	{
		return GetPin().mTypeID;
	}

	PinID PinProxy::GetID() const
	{
		return mPinID;
	}

	bool PinProxy::IsParentNodeReplacable() const
	{
		return Internal::IsNodeReplacable(NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), GetNodeID());
	}

	bool PinProxy::HasAnyConnectedLinks() const
	{
		return !GetConnectedPinIDs().empty();
	}

	bool PinProxy::IsViewAndEditable() const
	{
		return GenericDataTypeProxy(GetPinType().mGenericDataTypeID).IsViewAndEditable();
	}

	bool PinProxy::IsViewable() const
	{
		return GenericDataTypeProxy(GetPinType().mGenericDataTypeID).IsViewable();
	}

	bool PinProxy::IsSplitable() const
	{
		const Pin& pin = GetPin();
		return !pin.mIsSplit && !pin.mSubPinIDs.empty() && pin.mConnectedPinIDs.empty();
	}

	bool PinProxy::IsRecombinable() const
	{
		const Pin& pin = GetPin();
		if (pin.mParentPinID == InvalidID<PinID>())
		{
			return false;
		}
		return Internal::GetPin(pin.mParentPinID, Internal::GetNodeGraph(mNodeGraphVariant)).mIsSplit;
	}

	template<IsPredicate<const Pin&> Predicate>
	std::vector<PinProxy> GetPinsFiltered(Predicate&& aPredicate, const NodeGraphProxy& aNodeGraphProxy)
	{
		std::vector<PinProxy> pinProxys;
		const NodeGraph& nodeGraph = aNodeGraphProxy.GetNodeGraph();
		pinProxys.reserve(nodeGraph.mPins.size());

		for (PinID pinID{ 0 }; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinProxys.push_back(PinProxy(pinID, aNodeGraphProxy));
			}
		}

		return pinProxys;
	}

	std::vector<PinProxy> PinProxy::GetPotentialConnections() const
	{
		const eFlowType flowType = GetFlowType();
		const GenericDataTypeID dataTypeID = GetDataTypeID();
		return GetPinsFiltered(
			[flowType, dataTypeID](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinType(aPin.mTypeID);
				const bool a = aPin.mConnectedPinIDs.empty() && pinType.mFlowType == InvertFlowType(flowType);

				return a && Internal::AreDataTypesLinkable(SelectByFlowType(flowType, dataTypeID, pinType.mGenericDataTypeID), SelectByFlowType(flowType, pinType.mGenericDataTypeID, dataTypeID));
			},
			NodeGraphProxy(mNodeGraphVariant)
		);
	}

	/*std::vector<PinProxy> PinProxy::GetSplitPins() const
	{
		const Pin& pin = GetPin();

		if (!pin.mIsSplit || pin.mSubPinIDs.empty())
		{
			return { *this };
		}

		std::vector<PinProxy> pinProxys;
		for (const PinID subPinID : pin.mSubPinIDs)
		{
			PinProxy subPinProxy(subPinID, NodeGraphProxy(mNodeGraphVariant));
			std::vector<PinProxy> subPinSplitProxys = subPinProxy.GetSplitPins();
			pinProxys.insert(end(pinProxys), begin(subPinSplitProxys), end(subPinSplitProxys));
		}

		return pinProxys;
	}*/

	NodeGraphVariantHandle PinProxy::GetNodeGraphVariant() const
	{
		return mNodeGraphVariant;
	}

	void PinProxy::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinksByPin(Internal::GetNodeGraph(mNodeGraphVariant), mPinID, aCommandTracker);
	}

	void PinProxy::ViewAndEdit(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditPinGeneric(mPinID, Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	void PinProxy::View() const
	{
		Internal::ViewPinGeneric(mPinID, Internal::GetNodeGraph(mNodeGraphVariant));
	}

	void PinProxy::Split(CommandTracker* const aCommandTracker)
	{
		Internal::SplitPin(mPinID, Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	void PinProxy::RecombineParentPin(CommandTracker* const aCommandTracker)
	{
		Internal::RecombinePin(GetPin().mParentPinID, Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	PinProxy::operator bool() const
	{
		return mPinID != InvalidID<PinID>();
	}

	const Pin& PinProxy::GetPin() const
	{
		return NodeGraphProxy(mNodeGraphVariant).GetNodeGraph().mPins.at(mPinID);
	}

	const PinType& PinProxy::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Internal::GetPinTypeManager().GetPinType(pin.mTypeID);
	}

	bool operator==(const PinProxy& a, const PinProxy& b)
	{
		return a.mNodeGraphVariant == b.mNodeGraphVariant && a.mPinID == b.mPinID;
	}
}
