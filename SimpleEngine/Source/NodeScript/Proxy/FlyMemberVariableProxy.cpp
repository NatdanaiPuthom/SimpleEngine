#include "FlyMemberVariableProxy.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{
	MemberVariableProxy::MemberVariableProxy(const DataTypeID aParentDataTypeID, const size_t aMemberIndex)
		: mParentDataTypeID(aParentDataTypeID)
		, mMemberIndex(aMemberIndex)
	{
	}

	std::string_view MemberVariableProxy::GetName() const
	{
		return GetVariable().mName;
	}

	GenericDataTypeID MemberVariableProxy::GetDataTypeID() const
	{
		return GetVariable().mDataTypeID;
	}

	size_t MemberVariableProxy::GetByteOffset() const
	{
		return GetVariable().mByteOffset;
	}

	const DataType& MemberVariableProxy::GetParentDataType() const
	{
		return *Internal::GetDataTypeManager().Find(mParentDataTypeID);
	}

	const Variable& MemberVariableProxy::GetVariable() const
	{
		return Internal::GetDataTypeManager().Find(mParentDataTypeID)->mVariableContainer.mVariables[mMemberIndex];
	}
}