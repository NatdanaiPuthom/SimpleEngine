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

	ClassInstance& Class::CreateClassInstance()
	{
		return *mClassInstances.emplace_back(HeapObject<ClassInstance>(*this));
	}

	void Class::DestroyClassInstance(ClassInstance& aClassInstance)
	{
		std::erase_if(mClassInstances,
			[&aClassInstance](const HeapObject<ClassInstance>& aClassInstanceIter) -> bool
			{
				return &aClassInstance == &*aClassInstanceIter;
			}
		);
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

