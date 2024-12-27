#include "FlyNodeTypeFacade.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{
	NodeTypeFacade::NodeTypeFacade(const NodeTypeID anID)
		: mNodeTypeID(anID)
	{
	}

	const std::string& NodeTypeFacade::GetName() const
	{
		return GetNodeType().mNodeRecipe.mName;
	}

	std::string NodeTypeFacade::GetShortName() const
	{
		return Internal::GetNodeTypeManager().GetShortName(mNodeTypeID);
	}

	std::string NodeTypeFacade::GetNameDirectory() const
	{
		return Internal::GetNodeTypeManager().GetNameDirectory(mNodeTypeID);
	}

	eNodeTrait NodeTypeFacade::GetTraits() const
	{
		return GetNodeType().mNodeRecipe.mTraits;
	}

	EventID NodeTypeFacade::GetEventID() const
	{
		return GetNodeType().mNodeRecipe.mEventID;
	}

	std::vector<PinTypeFacade> NodeTypeFacade::GetInputPinTypeFacades() const
	{
		return GetPinTypes(eFlowType::Input);
	}

	std::vector<PinTypeFacade> NodeTypeFacade::GetOutputPinTypeFacades() const
	{
		return GetPinTypes(eFlowType::Output);
	}

	std::vector<DataTypeFacade> NodeTypeFacade::GetReplacableDataTypes() const
	{
		std::vector<DataTypeFacade> dataTypeFacades;

		auto& dataTypes = Internal::GetNodeTypeManager().GetTemplateMapByOperator(GetNodeType().mNodeRecipe.mOperatorTrait);
		dataTypeFacades.reserve(dataTypes.size());
		for (auto& [dataTypeID, nodeTypeID] : dataTypes)
		{
			dataTypeFacades.push_back(DataTypeFacade(dataTypeID));
		}

		return dataTypeFacades;
	}

	NodeTypeID NodeTypeFacade::GetID() const
	{
		return mNodeTypeID;
	}

	NodeTypeFacade::operator bool() const
	{
		return mNodeTypeID != InvalidID<NodeTypeID>();
	}

	bool NodeTypeFacade::operator==(const NodeTypeFacade& aOther) const
	{
		return mNodeTypeID == aOther.mNodeTypeID;
	}

	const NodeType& NodeTypeFacade::GetNodeType() const
	{
		return Internal::GetNodeTypeManager().GetNodeType(mNodeTypeID);
	}

	std::vector<PinTypeFacade> NodeTypeFacade::GetPinTypes(const eFlowType aFlowType) const
	{
		const NodeType& nodeType = GetNodeType();
		const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

		std::vector<PinTypeFacade> views;
		views.reserve(pinTypeIDs.size());

		for (const PinTypeID pinTypeID : pinTypeIDs)
		{
			views.emplace_back(PinTypeFacade(pinTypeID));
		}

		return views;
	}
}
