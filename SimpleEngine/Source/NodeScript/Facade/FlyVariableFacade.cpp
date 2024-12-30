#include "FlyVariableFacade.hpp"
#include "../DataType/FlyClass.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	VariableFacade::VariableFacade(const VarID aVarID, const ClassFacade& aClassFacade)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aClassFacade.GetID() })
	{
	}

	VariableFacade::VariableFacade(const VarID aVarID, const StructFacade& aStructFacade)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aStructFacade.GetID() })
	{
	}

	VariableFacade::VariableFacade(const VarID aVarID, const DataTypeFacade& aDataTypeFacade)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aDataTypeFacade.GetID() })
	{
	}

	// Temp
	static VariableContainer* GetVariableContainer(GenericDataTypeID aDataTypeID)
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID) -> VariableContainer* { assert(false); return nullptr; },
			[&](const StructID aStructID) -> VariableContainer* { return &Internal::GetStructByID(aStructID).mVariableContainer; },
			[&](const ClassID aClassID) -> VariableContainer* { return &Internal::GetClassByID(aClassID).mVariableContainer; }
			}, aDataTypeID.mID);
	}

	std::string_view VariableFacade::GetName() const
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID aDataTypeID) -> std::string_view { return Internal::GetDataTypeByID(aDataTypeID)->mVariables[mVarID].mName; },
			[&](const StructID aStructID) -> std::string_view { return Internal::GetStructByID(aStructID).mVariableContainer.mVariables[mVarID].mName; },
			[&](const ClassID aClassID) -> std::string_view { return Internal::GetClassByID(aClassID).mVariableContainer.mVariables[mVarID].mName; }
			}, mOwnerID.mID);
	}

	DataTypeID VariableFacade::GetDataTypeID() const
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID aDataTypeID) -> DataTypeID { return aDataTypeID; },
			[&](const StructID aStructID) -> DataTypeID { return Internal::GetStructByID(aStructID).mVariableContainer.mVariables[mVarID].mDataTypeID; },
			[&](const ClassID aClassID) -> DataTypeID { return Internal::GetClassByID(aClassID).mVariableContainer.mVariables[mVarID].mDataTypeID; }
			}, mOwnerID.mID);
	}

	bool VariableFacade::IsDestroyed() const
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID) -> bool { return false; },
			[&](const StructID aStructID) -> bool { return Internal::GetStructByID(aStructID).mVariableContainer.mVariables[mVarID].mIsDestroyed; },
			[&](const ClassID aClassID) -> bool { return Internal::GetClassByID(aClassID).mVariableContainer.mVariables[mVarID].mIsDestroyed; }
			}, mOwnerID.mID);
	}

	VarID VariableFacade::GetID() const
	{
		return mVarID;
	}

	size_t VariableFacade::GetByteOffset() const
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID aDataTypeID) -> size_t { return Internal::GetDataTypeByID(aDataTypeID)->mVariables[mVarID].mByteOffset; },
			[&](const StructID aStructID) -> size_t { assert(false); return Internal::GetStructByID(aStructID).mVariableContainer.mVariables[mVarID].mByteOffset; },
			[&](const ClassID aClassID) -> size_t { assert(false); return Internal::GetClassByID(aClassID).mVariableContainer.mVariables[mVarID].mByteOffset; }
			}, mOwnerID.mID);
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

	void VariableFacade::SetDataType(const DataTypeFacade aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(mVarID, *GetVariableContainer(mOwnerID), aDataTypeView.GetID(), aCommandTracker);
	}

	VariableFacade::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}
}
