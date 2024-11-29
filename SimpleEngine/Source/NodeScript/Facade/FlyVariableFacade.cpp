#include "FlyVariableFacade.hpp"
#include "FlyClass.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	VariableFacade::VariableFacade(const VarID aVarID, const ClassFacade& aClassView)
		: mVarID(aVarID)
		, mClass(&aClassView.GetClass())
	{
	}

	const std::string& VariableFacade::GetName() const
	{
		return GetVariable().mName;
	}

	DataTypeFacade VariableFacade::GetDataType() const
	{
		const DataTypeID dataTypeID = GetVariable().mDataTypeID;
		return DataTypeFacade(dataTypeID);
	}

	bool VariableFacade::IsDestroyed() const
	{
		return GetVariable().mIsDestroyed;
	}

	VarID VariableFacade::GetID() const
	{
		return mVarID;
	}

	Class& VariableFacade::GetClass() const
	{
		return *mClass;
	}

	void VariableFacade::SetName(const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableName(mVarID, *mClass, aName, aCommandTracker);
	}

	void VariableFacade::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyVariable(mVarID, *mClass, aCommandTracker);
	}

	void VariableFacade::EditDefaultValue(CommandTracker* const aCommandTracker)
	{
		Internal::EditVariableDefaultValue(mVarID, *mClass, aCommandTracker);
	}

	void VariableFacade::SetDataType(const DataTypeFacade aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(mVarID , *mClass, aDataTypeView.GetID(), aCommandTracker);
	}

	VariableFacade::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableFacade::GetVariable() const
	{
		return mClass->mStruct.mVariables.at(mVarID);
	}
}
