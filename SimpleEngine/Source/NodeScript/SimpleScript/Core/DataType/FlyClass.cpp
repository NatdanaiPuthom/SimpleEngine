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
		std::unique_ptr<ClassInstance>& classInstance = mClassInstances.emplace_back(std::make_unique<ClassInstance>(*this));
		return *classInstance;
	}

	void Class::DestroyClassInstance(ClassInstance& aClassInstance)
	{
		std::erase_if(mClassInstances,
			[&aClassInstance](const std::unique_ptr<ClassInstance>& aClassInstanceIter) -> bool
			{
				return &aClassInstance == aClassInstanceIter.get();
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

