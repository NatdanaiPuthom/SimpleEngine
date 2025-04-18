#include "FlyVariableProxy.hpp"
#include "../DataType/FlyDataType.hpp"
#include "../DataType/FlyClass.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	VariableProxy::VariableProxy(const VarID aVarID, const ClassProxy& aClass)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aClass.GetID() })
	{
	}

	VariableProxy::VariableProxy(const VarID aVarID, const DataTypeProxy& aDataType)
		: mVarID(aVarID)
		, mOwnerID(GenericDataTypeID{ aDataType.GetID() })
	{
	}

	VariableProxy::VariableProxy(VarID aVarID, const GenericDataTypeProxy& aGenericDataType)
		: mVarID(aVarID)
		, mOwnerID(aGenericDataType.GetID())
	{
	}

	// Temp
	static VariableContainer* GetVariableContainer(GenericDataTypeID aDataTypeID)
	{
		return std::visit(Visitor
			{
			[&](const DataTypeID aDataTypeID) -> VariableContainer* { return &Internal::GetDataTypeManager().Find(aDataTypeID)->GetVariableContainer(); },
			[&](const ClassID aClassID) -> VariableContainer* { return &Internal::GetClassByID(aClassID).mVariableContainer; }
			}, aDataTypeID.mID);
	}

	std::string_view VariableProxy::GetName() const
	{
		return Internal::GetDataTypeManager().Find(mOwnerID)->GetVariableContainer().GetVariable(mVarID).Name();
	}

	GenericDataTypeID VariableProxy::GetDataTypeID() const
	{
		return GetVariable().GetDataTypeID();
	}

	bool VariableProxy::IsDestroyed() const
	{
		return GetVariable().IsDestroyed();
	}

	VarID VariableProxy::GetID() const
	{
		return mVarID;
	}

	size_t VariableProxy::GetByteOffset() const
	{
		return GetVariable().GetByteOffset();
	}

	void VariableProxy::SetName(const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableName(mVarID, *GetVariableContainer(mOwnerID), aName, aCommandTracker);
	}

	void VariableProxy::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyVariable(mVarID, *GetVariableContainer(mOwnerID), aCommandTracker);
	}

	void VariableProxy::ViewAndEditDefaultValue(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditVariableDefaultValue(mVarID, *GetVariableContainer(mOwnerID), aCommandTracker);
	}

	void VariableProxy::SetDataType(const GenericDataTypeProxy aDataTypeProxy, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(mVarID, *GetVariableContainer(mOwnerID), aDataTypeProxy.GetID(), aCommandTracker);
	}

	VariableProxy::operator bool() const
	{
		return mVarID != InvalidID<VarID>();
	}

	const Variable& VariableProxy::GetVariable() const
	{
		return Internal::GetDataTypeByID(mOwnerID)->GetVariableContainer().GetVariable(mVarID);
	}
}
