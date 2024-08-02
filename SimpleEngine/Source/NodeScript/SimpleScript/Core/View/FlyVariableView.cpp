#include "FlyVariableView.hpp"
#include "FlyClass.hpp"

namespace FLY_NAMESPACE
{

	VariableView::VariableView(const VarID aVarID, Class& aClass)
		: mVarID(aVarID)
		, mClass(&aClass)
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

	VariableView::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableView::GetVariable() const
	{
		return mClass->mStruct.mVariables.at(mVarID);
	}
}
