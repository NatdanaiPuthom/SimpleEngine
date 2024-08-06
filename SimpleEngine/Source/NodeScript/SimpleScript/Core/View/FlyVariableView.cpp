#include "FlyVariableView.hpp"
#include "FlyClass.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{

	VariableView::VariableView(const VarID aVarID, const ClassView& aClassView)
		: mVarID(aVarID)
		, mClass(&aClassView.GetClass())
	{
	}

	const std::string& VariableView::GetName() const
	{
		return GetVariable().mName;
	}

	DataTypeView VariableView::GetDataType() const
	{
		const DataTypeID dataTypeID = GetVariable().mDataTypeID;
		return DataTypeView(dataTypeID);
	}

	bool VariableView::IsDestroyed() const
	{
		return GetVariable().mIsDestroyed;
	}

	VarID VariableView::GetID() const
	{
		return mVarID;
	}

	Class& VariableView::GetClass() const
	{
		return *mClass;
	}

	void VariableView::SetName(std::string_view aName)
	{
		SetVariableName(*this, aName, ClassView(*mClass));
	}

	VariableView::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableView::GetVariable() const
	{
		return mClass->mStruct.mVariables.at(mVarID);
	}
}
