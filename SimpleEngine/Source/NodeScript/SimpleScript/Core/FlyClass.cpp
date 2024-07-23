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

	std::string& Class::Name()
	{
		return mName;
	}

	const std::string& Class::Name() const
	{
		return mName;
	}

	ClassInstance& Class::CreateClassInstance()
	{
		std::unique_ptr<ClassInstance>& classInstance = mClassInstances.emplace_back(std::make_unique<ClassInstance>());
		classInstance->Init(*this);

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

	EventGraph& Class::GetEventGraph()
	{
		return mEventGraph;
	}

	const EventGraph& Class::GetEventGraph() const
	{
		return mEventGraph;
	}

	DataTypeID Class::GetTargetID() const
	{
		return mTargetID;
	}

	void Class::BindFunction(FunctionID aFunctionID)
	{
		mMemberFunctionIDs.push_back(aFunctionID);
	}

	const std::vector<FunctionID>& Class::GetMemberFunctionIDs() const
	{
		return mMemberFunctionIDs;
	}

	VariableManager& Class::GetVariableManager()
	{
		return mVariableManager;
	}

	const VariableManager& Class::GetVariableManager() const
	{
		return mVariableManager;
	}
}

