#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FlyGenericDataTypeFacade.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "FlyDataTypeFacade.hpp"

namespace FLY_NAMESPACE
{
	GenericDataTypeFacade::GenericDataTypeFacade(const GenericDataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
	}

	GenericDataTypeFacade::GenericDataTypeFacade(const DataTypeFacade& aDataTypeFacade)
		: mDataTypeID(GenericDataTypeID{ aDataTypeFacade.GetID() })
	{
	}

	GenericDataTypeFacade::GenericDataTypeFacade(const DataTypeID aDataTypeID)
		: GenericDataTypeFacade(GenericDataTypeID{ aDataTypeID })
	{
	}

	GenericDataTypeFacade::GenericDataTypeFacade(const StructID aStructID)
		: GenericDataTypeFacade(GenericDataTypeID{ aStructID })
	{
	}

	GenericDataTypeFacade::GenericDataTypeFacade(const ClassID aClassID)
		: GenericDataTypeFacade(GenericDataTypeID{ aClassID })
	{
	}

	GenericDataTypeID GenericDataTypeFacade::GetID() const
	{
		return mDataTypeID;
	}

	const std::string& GenericDataTypeFacade::GetName() const
	{
		return Internal::GetDataTypeManager().GetName(mDataTypeID);
	}

	Color GenericDataTypeFacade::GetColor() const
	{
		return Internal::GetDataTypeManager().GetDataTypeColor(mDataTypeID);
	}

	eDataTypeTrait GenericDataTypeFacade::GetTypeTraits() const
	{
		return Internal::GetDataTypeManager().GetDataTypeTraits(mDataTypeID);
	}

	size_t GenericDataTypeFacade::GetSize() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->mSize;
		}
		return 0;
	}

	size_t GenericDataTypeFacade::GetAlignment() const
	{
		return Internal::GetDataTypeManager().GetDataTypeAlignment(mDataTypeID);
	}

	bool GenericDataTypeFacade::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	bool GenericDataTypeFacade::IsPointer() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Pointer);
	}

	bool GenericDataTypeFacade::IsViewAndEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::ViewAndEditable);
	}

	bool GenericDataTypeFacade::IsViewable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Viewable);
	}

	std::vector<VariableFacade> GenericDataTypeFacade::GetMemberVariables() const
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

	void GenericDataTypeFacade::SetColor(const Color& aColor)
	{
		Internal::GetDataTypeManager().SetDataTypeColor(mDataTypeID, aColor);
	}

	GenericDataTypeFacade::operator bool() const
	{
		return mDataTypeID != InvalidID<GenericDataTypeID>();
	}

	bool GenericDataTypeFacade::operator==(const GenericDataTypeFacade& aOther) const
	{
		return mDataTypeID == aOther.mDataTypeID;
	}

	DataType* GenericDataTypeFacade::GetDataType() const
	{
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}
}