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

	Color DataTypeFacade::GetColor() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mColor;
		}
		return Global::GetDataTypeManager().GetDefaultColor();
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

	bool DataTypeFacade::IsPointer() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Pointer);
	}

	bool DataTypeFacade::IsViewAndEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::ViewAndEditable);
	}

	bool DataTypeFacade::IsViewable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Viewable);
	}


	std::optional<const std::vector<NodeTypeID>*> DataTypeFacade::GetNodeTypesIDs() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return std::nullopt;
		}
		return &dataType->mNodeTypeIDs;
	}

	void DataTypeFacade::SetColor(const Color& aColor)
	{
		GetDataType()->mColor = aColor;
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

	DataType* DataTypeFacade::GetDataType()
	{
		return Global::GetDataTypeManager().Find(mDataTypeID);
	}

	const DataType* DataTypeFacade::GetDataType() const
	{
		return Global::GetDataTypeManager().Find(mDataTypeID);
	}
}
