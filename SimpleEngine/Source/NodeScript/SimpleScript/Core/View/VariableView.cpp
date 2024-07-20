#include "VariableView.hpp"
#include "FlyClass.hpp"

namespace FLY_NAMESPACE
{

	VariableView::VariableView(const VarID aVarID, const Class& aClass)
		: myVarID(aVarID)
		, myClass(&aClass)
	{
	}

	const std::string& VariableView::GetName() const
	{
		return GetVariable().mName;
	}

	DataTypeView VariableView::GetDataType() const
	{
		const DataTypeID dataTypeID = GetVariable().dataTypeID;
		return DataTypeView(dataTypeID);
	}

	bool VariableView::IsDestroyed() const
	{
		return GetVariable().mIsDestroyed;
	}

	VarID VariableView::GetID() const
	{
		return myVarID;
	}

	const Variable& VariableView::GetVariable() const
	{
		return myClass->GetVariableManager().mVariables.at(myVarID);
	}
}
