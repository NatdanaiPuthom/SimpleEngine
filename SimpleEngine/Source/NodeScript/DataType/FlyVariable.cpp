#include "FlyVariable.hpp"

namespace FLY_NAMESPACE
{
	Variable::Variable(GenericDataTypeID aDataTypeID, std::string aName, size_t aByteOffset)
		: mDataTypeID(std::move(aDataTypeID))
		, mName(std::move(aName))
		, mByteOffset(aByteOffset)
	{
	}

	void Variable::SetDataTypeID(GenericDataTypeID aDataTypeID)
	{
		mDataTypeID = std::move(aDataTypeID);
	}

	std::string& Variable::Name()
	{
		return mName;
	}

	const std::string& Variable::Name() const
	{
		return mName;
	}

	void Variable::SetIsDestroyed(bool aIsDestroyed)
	{
		mIsDestroyed = aIsDestroyed;
	}

	bool Variable::IsDestroyed() const
	{
		return mIsDestroyed;
	}
}