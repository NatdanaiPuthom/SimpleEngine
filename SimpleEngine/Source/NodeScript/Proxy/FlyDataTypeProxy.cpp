#include "FlyDataTypeProxy.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	DataTypeProxy::DataTypeProxy(const DataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			mDataTypeID = InvalidID<DataTypeID>();
		}
	}

	DataTypeID DataTypeProxy::GetID() const
	{
		return mDataTypeID;
	}

	const std::string& DataTypeProxy::GetName() const
	{
		return Internal::GetDataTypeManager().GetName(mDataTypeID);
	}

	Color DataTypeProxy::GetColor() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mColor;
		}
		return Internal::GetDataTypeManager().GetDefaultColor();
	}

	eDataTypeTrait DataTypeProxy::GetTypeTraits() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mTypeTraits;
		}
		return eDataTypeTrait::None;
	}

	size_t DataTypeProxy::GetSize() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mSize;
		}
		return 0;
	}

	size_t DataTypeProxy::GetAlignment() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mAlignment;
		}
		return 0;
	}

	bool DataTypeProxy::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	bool DataTypeProxy::IsPointer() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Pointer);
	}

	bool DataTypeProxy::IsViewAndEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::ViewAndEditable);
	}

	bool DataTypeProxy::IsViewable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Viewable);
	}

	std::vector<VariableProxy> DataTypeProxy::GetMemberVariables() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return {};
		}

		std::vector<VariableProxy> memberVariables;
		memberVariables.reserve(dataType->mVariableContainer.mVariables.size());
		for (VarID varID{ 0 }; varID < dataType->mVariableContainer.mVariables.size(); ++varID)
		{
			VariableProxy facade(varID, *this);
			memberVariables.push_back(facade);
		}
		return memberVariables;
	}


	std::optional<const std::vector<NodeTypeID>*> DataTypeProxy::GetNodeTypesIDs() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return std::nullopt;
		}
		return &dataType->mNodeTypeIDs;
	}

	void DataTypeProxy::SetColor(const Color& aColor)
	{
		GetDataType()->mColor = aColor;
	}

	DataTypeProxy::operator bool() const
	{
		return mDataTypeID != InvalidID<DataTypeID>();
	}

	bool operator==(const DataTypeProxy& a, const DataTypeProxy& b)
	{
		return a.mDataTypeID == b.mDataTypeID;
	}

	DataType* DataTypeProxy::GetDataType()
	{
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}

	const DataType* DataTypeProxy::GetDataType() const
	{
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}
}
