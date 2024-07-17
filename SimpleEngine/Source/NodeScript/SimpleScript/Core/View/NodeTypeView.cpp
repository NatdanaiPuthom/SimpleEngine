#include "NodeTypeView.h"
#include "../Node/NodeTypeManager.h"
#include "../Global/ScriptGlobal.h"

namespace SCR
{
	NodeTypeView::NodeTypeView(NodeTypeID anID)
		: myNodeTypeID(anID)
	{
	}

	const std::string& NodeTypeView::GetName() const
	{
		return GetNodeType().name;
	}

	std::string NodeTypeView::GetShortName() const
	{
		return Global::GetNodeTypeManager().GetShortName(myNodeTypeID);
	}

	std::string NodeTypeView::GetNameDirectory() const
	{
		return Global::GetNodeTypeManager().GetNameDirectory(myNodeTypeID);
	}

	eNodeTrait NodeTypeView::GetTraits() const
	{
		return GetNodeType().nodeRecipe.traits;
	}

	EventID NodeTypeView::GetEventID() const
	{
		return GetNodeType().nodeRecipe.eventID;
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
		return myNodeTypeID;
	}

	bool NodeTypeView::operator==(const NodeTypeView& aOther) const
	{
		return myNodeTypeID == aOther.myNodeTypeID;
	}

	bool NodeTypeView::operator!=(const NodeTypeView& aOther) const
	{
		return !(*this == aOther);
	}

	const NodeType& NodeTypeView::GetNodeType() const
	{
		return Global::GetNodeTypeManager().GetNodeType(myNodeTypeID);
	}

	std::vector<PinTypeView> NodeTypeView::GetPinTypes(const eFlowType aFlowType) const
	{
		const NodeType& nodeType = GetNodeType();
		const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.nodeRecipe.inputPinTypeIDs, nodeType.nodeRecipe.outputPinTypeIDs);

		std::vector<PinTypeView> views;
		views.reserve(pinTypeIDs.size());

		for (const PinTypeID pinTypeID : pinTypeIDs)
		{
			views.emplace_back(PinTypeView(pinTypeID));
		}

		return views;
	}
}
