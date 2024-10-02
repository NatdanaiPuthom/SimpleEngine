#include "FlyDataTypeFacade.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeFacade::DataTypeFacade()
		: DataTypeFacade(GetDataTypeID<None>())
	{
	}

	DataTypeFacade::DataTypeFacade(const DataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			mDataTypeID = InvalidID<DataTypeID>();
		}
	}

	DataTypeID DataTypeFacade::GetID() const
	{
		return mDataTypeID;
	}

	const std::string& DataTypeFacade::GetName() const
	{
		return Global::GetDataTypeManager().GetName(mDataTypeID);
	}

	const Color& DataTypeFacade::GetColor() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mColor;
		}
		return DefaultColor;
	}

	eDataTypeTrait DataTypeFacade::GetTypeTraits() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mTypeTraits;
		}
		return eDataTypeTrait::None;
	}

	bool DataTypeFacade::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	bool DataTypeFacade::IsEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Editable);
	}

	std::vector<NodeTypeFacade> DataTypeFacade::GetNodeTypes() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return std::vector<NodeTypeFacade>();
		}
		const std::vector<NodeTypeID>& nodeTypeIDs = dataType->mNodeTypeIDs;

		std::vector<NodeTypeFacade> nodeTypeFacades;

		nodeTypeFacades.reserve(nodeTypeIDs.size());

		for (const NodeTypeID nodeTypeID : nodeTypeIDs)
		{
			nodeTypeFacades.push_back(NodeTypeFacade(nodeTypeID));
		}

		return nodeTypeFacades;
	}

	DataTypeFacade::operator bool() const
	{
		return mDataTypeID != InvalidID<DataTypeID>();
	}

	bool DataTypeFacade::operator==(const DataTypeFacade& aOther) const
	{
		return mDataTypeID == aOther.mDataTypeID;
	}

	bool DataTypeFacade::operator!=(const DataTypeFacade& aOther) const
	{
		return !(*this == aOther);
	}

	const DataType* DataTypeFacade::GetDataType() const
	{
		return Global::GetDataTypeManager().Find(mDataTypeID);
	}
}
