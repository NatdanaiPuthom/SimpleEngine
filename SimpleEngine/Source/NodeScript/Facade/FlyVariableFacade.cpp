#include "FlyVariableFacade.hpp"
#include "../DataType/FlyDataType.hpp"
#include "../DataType/FlyClass.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	VariableFacade::VariableFacade(const VarID aVarID, const ClassFacade& aClassFacade)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aClassFacade.GetID() })
	{
	}

	VariableFacade::VariableFacade(const VarID aVarID, const DataTypeFacade& aDataTypeFacade)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aDataTypeFacade.GetID() })
	{
	}

	VariableFacade::VariableFacade(VarID aVarID, const GenericDataTypeFacade& aGenericDataTypeFacade)
		: mVarID(aVarID)
		, mOwnerID(aGenericDataTypeFacade.GetID())
	{
	}

	// Temp
	static VariableContainer* GetVariableContainer(GenericDataTypeID aDataTypeID)
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID aDataTypeID) -> VariableContainer* { return &Internal::GetDataTypeManager().Find(aDataTypeID)->mVariableContainer; },
			[&](const ClassID aClassID) -> VariableContainer* { return &Internal::GetClassByID(aClassID).mVariableContainer; }
			}, aDataTypeID.mID);
	}

	std::string_view VariableFacade::GetName() const
	{
		return Internal::GetDataTypeManager().Find(mOwnerID)->mVariableContainer.mVariables[mVarID].mName;
	}

	GenericDataTypeID VariableFacade::GetDataTypeID() const
	{
		return GetVariable().mDataTypeID;
	}

	bool VariableFacade::IsDestroyed() const
	{
		return GetVariable().mIsDestroyed;
	}

	VarID VariableFacade::GetID() const
	{
		return mVarID;
	}

	size_t VariableFacade::GetByteOffset() const
	{
		return GetVariable().mByteOffset;
	}

	void VariableFacade::SetName(const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableName(mVarID, *GetVariableContainer(mOwnerID), aName, aCommandTracker);
	}

	void VariableFacade::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyVariable(mVarID, *GetVariableContainer(mOwnerID), aCommandTracker);
	}

	void VariableFacade::ViewAndEditDefaultValue(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditVariableDefaultValue(mVarID, *GetVariableContainer(mOwnerID), aCommandTracker);
	}

	void VariableFacade::SetDataType(const GenericDataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(mVarID, *GetVariableContainer(mOwnerID), aDataTypeFacade.GetID(), aCommandTracker);
	}

	VariableFacade::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableFacade::GetVariable() const
	{
		return Internal::GetDataTypeByID(mOwnerID)->mVariableContainer.mVariables[mVarID];
	}
}
