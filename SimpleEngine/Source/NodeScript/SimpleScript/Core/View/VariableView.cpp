#include "VariableView.hpp"
#include "FlyClass.hpp"

namespace SCR
{

	VariableView::VariableView(VarID aVarID, const Class& aClass)
		: myVarID(aVarID)
		, myClass(&aClass)
	{
	}

	const std::string& VariableView::GetName() const
	{
		return GetVariable().mName;
	}

	DataTypeID VariableView::GetDataTypeID() const
	{
		return GetVariable().dataTypeID;
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
