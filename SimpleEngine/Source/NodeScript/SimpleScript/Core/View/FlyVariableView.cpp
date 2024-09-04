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

	void VariableView::SetName(const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		SetVariableName(*this, aName, aCommandTracker);
	}

	void VariableView::Destroy(CommandTracker* const aCommandTracker)
	{
		DestroyVariable(*this, aCommandTracker);
	}

	void VariableView::EditDefaultValue(CommandTracker* const aCommandTracker)
	{
		EditVariableDefaultValue(*this, aCommandTracker);
	}

	void VariableView::SetDataType(const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		SetVariableDataType(*this, aDataTypeView, aCommandTracker);
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
