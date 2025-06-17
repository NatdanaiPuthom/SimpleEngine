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
		return GetPin().GetConnectedPinIDs();
	}

	GenericDataTypeID PinProxy::GetDataTypeID() const
	{
		return GetPinType().GetDataTypeID();
	}

	std::string_view PinProxy::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.GetName() == TypeIdentifierStr)
		{
			return Internal::GetDataTypeManager().GetName(pinType.GetDataTypeID());
		}
		return pinType.GetName();
	}

	eIODirection PinProxy::GetIODirection() const
	{
		return GetPinType().GetIODirection();
	}

	NodeID PinProxy::GetNodeID() const
	{
		return GetPin().GetNodeID();
	}

	PinTypeID PinProxy::GetPinTypeID() const
	{
		return GetPin().GetTypeID();
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
		return GenericDataTypeProxy(GetPinType().GetDataTypeID()).IsViewAndEditable();
	}

	bool PinProxy::IsViewable() const
	{
		return GenericDataTypeProxy(GetPinType().GetDataTypeID()).IsViewable();
	}

	bool PinProxy::IsSplitable() const
	{
		const Pin& pin = GetPin();
		return !pin.IsSplit() && !pin.GetSplitPinIDs().empty() && pin.GetConnectedPinIDs().empty();
	}

	bool PinProxy::IsRecombinable() const
	{
		const Pin& pin = GetPin();
		if (pin.GetParentPinID() == InvalidID<PinID>())
		{
			return false;
		}
		return Internal::GetNodeGraph(mNodeGraphVariant).GetPin(pin.GetParentPinID()).IsSplit();
	}

	template<IsPredicate<const Pin&> Predicate>
	std::vector<PinProxy> GetPinsFiltered(Predicate&& aPredicate, const NodeGraphProxy& aNodeGraphProxy)
	{
		std::vector<PinProxy> pinProxys;
		const NodeGraph& nodeGraph = aNodeGraphProxy.GetNodeGraph();
		pinProxys.reserve(nodeGraph.GetPinCount());

		for (PinID pinID{ 0 }; pinID < nodeGraph.GetPinCount(); ++pinID)
		{
			const Pin& pin = nodeGraph.GetPin(pinID);
			if (aPredicate(pin))
			{
				pinProxys.push_back(PinProxy(pinID, aNodeGraphProxy));
			}
		}

		return pinProxys;
	}

	std::vector<PinProxy> PinProxy::GetPotentialConnections() const
	{
		const eIODirection ioDirection = GetIODirection();
		const GenericDataTypeID dataTypeID = GetDataTypeID();
		return GetPinsFiltered(
			[ioDirection, dataTypeID](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinType(aPin.GetTypeID());
				const bool a = aPin.GetConnectedPinIDs().empty() && pinType.GetIODirection() == InvertIODirection(ioDirection);

				return a && Internal::AreDataTypesLinkable(SelectByIODirection(ioDirection, dataTypeID, pinType.GetDataTypeID()), SelectByIODirection(ioDirection, pinType.GetDataTypeID(), dataTypeID));
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
		Internal::RecombinePin(GetPin().GetParentPinID(), Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	PinProxy::operator bool() const
	{
		return mPinID != InvalidID<PinID>();
	}

	const Pin& PinProxy::GetPin() const
	{
		return NodeGraphProxy(mNodeGraphVariant).GetNodeGraph().GetPin(mPinID);
	}

	const PinType& PinProxy::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Internal::GetPinTypeManager().GetPinType(pin.GetTypeID());
	}

	bool operator==(const PinProxy& a, const PinProxy& b)
	{
		return a.mNodeGraphVariant == b.mNodeGraphVariant && a.mPinID == b.mPinID;
	}
}
