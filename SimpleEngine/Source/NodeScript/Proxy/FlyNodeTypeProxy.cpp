#include "FlyNodeTypeProxy.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{
	NodeTypeProxy::NodeTypeProxy(const NodeTypeID aID)
		: mNodeTypeID(aID)
	{
	}

	const std::string& NodeTypeProxy::GetName() const
	{
		return GetNodeType().GetName();
	}

	std::string NodeTypeProxy::GetShortName() const
	{
		return Internal::GetNodeTypeManager().GetShortName(mNodeTypeID);
	}

	std::string NodeTypeProxy::GetNameDirectory() const
	{
		return Internal::GetNodeTypeManager().GetNameDirectory(mNodeTypeID);
	}

	eNodeTrait NodeTypeProxy::GetTraits() const
	{
		return GetNodeType().GetTraits();
	}

	EventID NodeTypeProxy::GetEventID() const
	{
		return GetNodeType().GetEventID();
	}

	std::vector<PinTypeProxy> NodeTypeProxy::GetInputPinTypes() const
	{
		return GetPinTypes(eIODirection::Input);
	}

	std::vector<PinTypeProxy> NodeTypeProxy::GetOutputPinTypes() const
	{
		return GetPinTypes(eIODirection::Output);
	}

	std::vector<DataTypeProxy> NodeTypeProxy::GetReplacableDataTypes(const bool aUseTraits) const
	{
		std::vector<DataTypeProxy> dataTypeProxys;
		if (aUseTraits)
		{
			auto& dataTypes = Internal::GetNodeTypeManager().GetMapByTrait(GetNodeType().GetTraitID());
			dataTypeProxys.reserve(dataTypes.size());
			for (auto& [dataTypeID, nodeTypeID] : dataTypes)
			{
				dataTypeProxys.push_back(DataTypeProxy(dataTypeID));
			}
		}
		else
		{


			auto& dataTypes = Internal::GetNodeTypeManager().GetTemplateMapByOperator(GetNodeType().GetOperatorTrait());
			dataTypeProxys.reserve(dataTypes.size());
			for (auto& [dataTypeID, nodeTypeID] : dataTypes)
			{
				dataTypeProxys.push_back(DataTypeProxy(dataTypeID));
			}
		}

		return dataTypeProxys;
	}

	NodeTypeID NodeTypeProxy::GetID() const
	{
		return mNodeTypeID;
	}

	NodeTypeProxy::operator bool() const
	{
		return mNodeTypeID != InvalidID<NodeTypeID>();
	}

	bool operator==(const NodeTypeProxy& a, const NodeTypeProxy& b)
	{
		return a.mNodeTypeID == b.mNodeTypeID;
	}

	const NodeType& NodeTypeProxy::GetNodeType() const
	{
		return Internal::GetNodeTypeManager().GetNodeType(mNodeTypeID);
	}

	std::vector<PinTypeProxy> NodeTypeProxy::GetPinTypes(const eIODirection aIODirection) const
	{
		const NodeType& nodeType = GetNodeType();
		const std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, nodeType.GetInputPinTypeIDs(), nodeType.GetOutputPinTypeIDs());

		std::vector<PinTypeProxy> views;
		views.reserve(pinTypeIDs.size());

		for (const PinTypeID pinTypeID : pinTypeIDs)
		{
			views.emplace_back(PinTypeProxy(pinTypeID));
		}

		return views;
	}
}
