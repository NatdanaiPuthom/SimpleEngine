#include "FlyClass.hpp"
#include "Utilities/ScriptProxy.hpp"
#include "Node/NodeExecutor.hpp"
#include "Command/ScriptCommandTracker.hpp"
#include "ScriptUtilities.hpp"
#include "Instance/FlyClassInstance.hpp"
#include "Function/ScriptFunction.hpp"

namespace SCR
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

	const VariableManager& Class::GetVariableManager() const
	{
		return mVariableManager;
	}
}

