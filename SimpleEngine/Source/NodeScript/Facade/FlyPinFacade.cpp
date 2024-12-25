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
		return !GetConnectedPinIDs().empty();
	}

	bool PinFacade::IsViewAndEditable() const
	{
		return DataTypeFacade(GetPinType().mDataTypeID).IsViewAndEditable();
	}

	bool PinFacade::IsViewable() const
	{
		return DataTypeFacade(GetPinType().mDataTypeID).IsViewable();
	}

	bool PinFacade::IsSplitable() const
	{
		const Pin& pin = GetPin();
		return !pin.mIsSplit && !pin.mSubPinIDs.empty() && pin.mConnectedPinIDs.empty();
	}

	bool PinFacade::IsRecombinable() const
	{
		const Pin& pin = GetPin();
		if (pin.mParentPinID == InvalidID<PinID>())
		{
			return false;
		}
		return Internal::GetPin(pin.mParentPinID, Internal::GetNodeGraph(mNodeGraphVariant)).mIsSplit;
	}

	template<Predicate<const Pin&> Predicate>
	std::vector<PinFacade> GetPinFacadesFiltered(Predicate&& aPredicate, const NodeGraphFacade& aNodeGraphFacade)
	{
		std::vector<PinFacade> pinFacades;
		const NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();
		pinFacades.reserve(nodeGraph.mPins.size());

		for (PinID pinID = 0; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinFacades.push_back(PinFacade(pinID, aNodeGraphFacade));
			}
		}

		return pinFacades;
	}

	std::vector<PinFacade> PinFacade::GetPotentialConnections() const
	{
		const eFlowType flowType = GetFlowType();
		const DataTypeID dataTypeID = GetDataTypeID();
		return GetPinFacadesFiltered(
			[flowType, dataTypeID](const Pin& aPin) -> bool
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
				const bool a = aPin.mConnectedPinIDs.empty() && pinType.mFlowType == InvertFlowType(flowType);

				return a && Internal::AreDataTypesLinkable(SelectByFlowType(flowType, dataTypeID, pinType.mDataTypeID), SelectByFlowType(flowType, pinType.mDataTypeID, dataTypeID));
			},
			NodeGraphFacade(mNodeGraphVariant)
		);
	}

	/*std::vector<PinFacade> PinFacade::GetSplitPins() const
	{
		const Pin& pin = GetPin();

		if (!pin.mIsSplit || pin.mSubPinIDs.empty())
		{
			return { *this };
		}

		std::vector<PinFacade> pinFacades;
		for (const PinID subPinID : pin.mSubPinIDs)
		{
			PinFacade subPinFacade(subPinID, NodeGraphFacade(mNodeGraphVariant));
			std::vector<PinFacade> subPinSplitFacades = subPinFacade.GetSplitPins();
			pinFacades.insert(end(pinFacades), begin(subPinSplitFacades), end(subPinSplitFacades));
		}

		return pinFacades;
	}*/

	NodeGraphVariantHandle PinFacade::GetNodeGraphVariant() const
	{
		return mNodeGraphVariant;
	}

	void PinFacade::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinksByPin(Internal::GetNodeGraph(mNodeGraphVariant), mPinID, aCommandTracker);
	}

	void PinFacade::ViewAndEdit(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditPin(GetID(), Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	void PinFacade::View() const
	{
		Internal::ViewPin(mPinID, Internal::GetNodeGraph(mNodeGraphVariant));
	}

	void PinFacade::Split(CommandTracker* const aCommandTracker)
	{
		Internal::SplitPin(mPinID, Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
	}

	void PinFacade::RecombineParentPin(CommandTracker* const aCommandTracker)
	{
		Internal::RecombinePin(GetPin().mParentPinID, Internal::GetNodeGraph(mNodeGraphVariant), aCommandTracker);
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
		return a.mNodeGraphVariant == b.mNodeGraphVariant && a.mPinID == b.mPinID;
	}
}
