#include "FlyVariableFacade.hpp"
#include "FlyClass.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	VariableFacade::VariableFacade(const VarID aVarID, const ClassFacade& aClassFacade)
		: mVarID(aVarID)
		, mOwner(&aClassFacade.GetVariableContainer())
	{
	}

	VariableFacade::VariableFacade(VarID aVarID, const StructFacade& aStructFacade)
		: mVarID(aVarID)
		, mOwner(&aStructFacade.GetVariableContainer())
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

	void VariableFacade::SetName(const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableName(mVarID, *mOwner, aName, aCommandTracker);
	}

	void VariableFacade::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyVariable(mVarID, *mOwner, aCommandTracker);
	}

	void VariableFacade::ViewAndEditDefaultValue(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditVariableDefaultValue(mVarID, *mOwner, aCommandTracker);
	}

	void VariableFacade::SetDataType(const DataTypeFacade aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(mVarID , *mOwner, aDataTypeView.GetID(), aCommandTracker);
	}

	VariableFacade::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableFacade::GetVariable() const
	{
		return mOwner->mVariables.at(mVarID);
	}
}
