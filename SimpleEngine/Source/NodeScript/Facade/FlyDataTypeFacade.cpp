#include "FlyDataTypeFacade.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

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
		return Internal::GetDataTypeManager().GetName(mDataTypeID);
	}

	Color DataTypeFacade::GetColor() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mColor;
		}
		return Internal::GetDataTypeManager().GetDefaultColor();
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

	size_t DataTypeFacade::GetSize() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mSize;
		}
		return 0;
	}

	size_t DataTypeFacade::GetAlignment() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mAlignment;
		}
		return 0;
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

	std::vector<VariableFacade> DataTypeFacade::GetMemberVariables() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return {};
		}

		std::vector<VariableFacade> memberVariables;
		memberVariables.reserve(dataType->mVariableContainer.mVariables.size());
		for (VarID varID{ 0 }; varID < dataType->mVariableContainer.mVariables.size(); ++varID)
		{
			VariableFacade facade(varID, *this);
			memberVariables.push_back(facade);
		}
		return memberVariables;
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
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}

	const DataType* DataTypeFacade::GetDataType() const
	{
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}
}
