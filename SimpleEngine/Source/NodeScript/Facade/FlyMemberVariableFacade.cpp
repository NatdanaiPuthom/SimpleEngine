#include "FlyMemberVariableFacade.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{
	MemberVariableFacade::MemberVariableFacade(const DataTypeID aParentDataTypeID, const size_t aMemberIndex)
		: mParentDataTypeID(aParentDataTypeID)
		, mMemberIndex(aMemberIndex)
	{
	}

	std::string_view MemberVariableFacade::GetName() const
	{
		return GetVariable().mName;
	}

	DataTypeID MemberVariableFacade::GetDataTypeID() const
	{
		return GetVariable().mDataTypeID;
	}

	size_t MemberVariableFacade::GetByteOffset() const
	{
		return GetVariable().mByteOffset;
	}

	const DataType& MemberVariableFacade::GetParentDataType() const
	{
		return *Internal::GetDataTypeManager().Find(mParentDataTypeID);
	}

	const Variable& MemberVariableFacade::GetVariable() const
	{
		return Internal::GetDataTypeManager().Find(mParentDataTypeID)->mVariables[mMemberIndex];
	}
}