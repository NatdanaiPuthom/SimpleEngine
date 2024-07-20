#include "FlyDataTypeView.hpp"
#include "../Global/ScriptGlobal.hpp"
#include "../DataType/DataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeView::DataTypeView()
		: DataTypeView(GetDataTypeID<None>())
	{
	}

	DataTypeView::DataTypeView(const DataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
		if (Global::GetDataTypeManager().Find(mDataTypeID) == nullptr)
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
		return GetDataType().mName;
	}

	const Color& DataTypeView::GetColor() const
	{
		return GetDataType().mColor;
	}

	eDataTypeTrait DataTypeView::GetTypeTraits() const
	{
		return GetDataType().mTypeTraits;
	}

	bool DataTypeView::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	std::vector<NodeTypeView> DataTypeView::GetNodeTypes() const
	{
		const std::vector<NodeTypeID>& nodeTypeIDs = GetDataType().mNodeTypeIDs;
		
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

	const DataType& DataTypeView::GetDataType() const
	{
		const DataType* dataType = Global::GetDataTypeManager().Find(mDataTypeID);
		assert(dataType);
		return *dataType;
	}
}
