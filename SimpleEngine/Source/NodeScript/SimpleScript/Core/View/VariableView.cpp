#include "VariableView.h"
#include "Script.h"

namespace SCR
{

	VariableView::VariableView(VarID aVarID, const Script& aScript)
		: myVarID(aVarID)
		, myScript(&aScript)
	{
	}

	const std::string& VariableView::GetName() const
	{
		return GetVariable().name;
	}

	DataTypeID VariableView::GetDataTypeID() const
	{
		return GetVariable().dataTypeID;
	}

	bool VariableView::IsDestroyed() const
	{
		return GetVariable().isDestroyed;
	}

	VarID VariableView::GetID() const
	{
		return myVarID;
	}

	const Variable& VariableView::GetVariable() const
	{
		return myScript->GetVariableManager().myVariables.at(myVarID);
	}
}
