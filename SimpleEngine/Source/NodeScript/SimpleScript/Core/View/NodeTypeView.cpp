#include "NodeTypeView.hpp"
#include "../Node/NodeTypeManager.hpp"
#include "../Global/ScriptGlobal.hpp"

namespace FLY_NAMESPACE
{
	NodeTypeView::NodeTypeView(NodeTypeID anID)
		: mNodeTypeID(anID)
	{
	}

	const std::string& NodeTypeView::GetName() const
	{
		return GetNodeType().mName;
	}

	std::string NodeTypeView::GetShortName() const
	{
		return Global::GetNodeTypeManager().GetShortName(mNodeTypeID);
	}

	std::string NodeTypeView::GetNameDirectory() const
	{
		return Global::GetNodeTypeManager().GetNameDirectory(mNodeTypeID);
	}

	eNodeTrait NodeTypeView::GetTraits() const
	{
		return GetNodeType().mNodeRecipe.mTraits;
	}

	EventID NodeTypeView::GetEventID() const
	{
		return GetNodeType().mNodeRecipe.mEventID;
	}

	std::vector<PinTypeView> NodeTypeView::GetInputPinTypes() const
	{
		return GetPinTypes(eFlowType::Input);
	}

	std::vector<PinTypeView> NodeTypeView::GetOutputPinTypes() const
	{
		return GetPinTypes(eFlowType::Output);
	}

	NodeTypeID NodeTypeView::GetID() const
	{
		return mNodeTypeID;
	}

	bool NodeTypeView::operator==(const NodeTypeView& aOther) const
	{
		return mNodeTypeID == aOther.mNodeTypeID;
	}

	bool NodeTypeView::operator!=(const NodeTypeView& aOther) const
	{
		return !(*this == aOther);
	}

	const NodeType& NodeTypeView::GetNodeType() const
	{
		return Global::GetNodeTypeManager().GetNodeType(mNodeTypeID);
	}

	std::vector<PinTypeView> NodeTypeView::GetPinTypes(const eFlowType aFlowType) const
	{
		const NodeType& nodeType = GetNodeType();
		const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

		std::vector<PinTypeView> views;
		views.reserve(pinTypeIDs.size());

		for (const PinTypeID pinTypeID : pinTypeIDs)
		{
			views.emplace_back(PinTypeView(pinTypeID));
		}

		return views;
	}
}
