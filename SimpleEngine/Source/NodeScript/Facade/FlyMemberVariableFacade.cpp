#include "FlyMemberVariableFacade.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

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
		return *Global::GetDataTypeManager().Find(mParentDataTypeID);
	}

	const Variable& MemberVariableFacade::GetVariable() const
	{
		return Global::GetDataTypeManager().Find(mParentDataTypeID)->mVariables[mMemberIndex];
	}
}