#include "FlyClass.hpp"
#include "Instance/FlyClassInstance.hpp"

namespace FLY_NAMESPACE
{

	Class::Class(const DataTypeID aTargetID, const std::string& aName)
		: mTargetID(aTargetID)
		, mName(aName)
	{
	}

	Class::~Class()
	{
	}

	void Class::BindFunction(FunctionID aFunctionID)
	{
		mMemberFunctionIDs.push_back(aFunctionID);
	}

	const std::vector<FunctionID>& Class::GetMemberFunctionIDs() const
	{
		return mMemberFunctionIDs;
	}
}

