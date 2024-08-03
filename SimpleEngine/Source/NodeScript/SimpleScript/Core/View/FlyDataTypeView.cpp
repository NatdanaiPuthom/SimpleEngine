#include "FlyDataTypeView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeView::DataTypeView()
		: DataTypeView(GetDataTypeID<None>())
	{
	}

	DataTypeView::DataTypeView(const DataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			mDataTypeID = InvalidID<DataTypeID>();
		}
	}

	DataTypeID DataTypeView::GetID() const
	{
		return mDataTypeID;
	}

	const std::string& DataTypeView::GetName() const
	{
		return Global::GetDataTypeManager().GetName(mDataTypeID);
	}

	const Color& DataTypeView::GetColor() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mColor;
		}
		return DefaultColor;
	}

	eDataTypeTrait DataTypeView::GetTypeTraits() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mTypeTraits;
		}
		return eDataTypeTrait::None;
	}

	bool DataTypeView::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	bool DataTypeView::IsEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Editable);
	}

	std::vector<NodeTypeView> DataTypeView::GetNodeTypes() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return std::vector<NodeTypeView>();
		}
		const std::vector<NodeTypeID>& nodeTypeIDs = dataType->mNodeTypeIDs;

		std::vector<NodeTypeView> nodeTypeViews;

		nodeTypeViews.reserve(nodeTypeIDs.size());

		for (const NodeTypeID nodeTypeID : nodeTypeIDs)
		{
			nodeTypeViews.push_back(NodeTypeView(nodeTypeID));
		}

		return nodeTypeViews;
	}

	DataTypeView::operator bool() const
	{
		return mDataTypeID != InvalidID<DataTypeID>();
	}

	bool DataTypeView::operator==(const DataTypeView& aOther) const
	{
		return mDataTypeID == aOther.mDataTypeID;
	}

	bool DataTypeView::operator!=(const DataTypeView& aOther) const
	{
		return !(*this == aOther);
	}

	const DataType* DataTypeView::GetDataType() const
	{
		return Global::GetDataTypeManager().Find(mDataTypeID);
	}
}
