#include "FlyClassInstanceFacade.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Execution/FlyNodeExecutor.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"

namespace FLY_NAMESPACE
{

	ClassInstanceFacade::ClassInstanceFacade(ClassInstance& aClassInstance)
		: mClassInstance(&aClassInstance)
	{
	}

	void ClassInstanceFacade::InitRuntime()
	{
		mClassInstance->InitRuntime();
	}

	const std::string& ClassInstanceFacade::GetName() const
	{
		return mClassInstance->mClass->mName;
	}

	ClassInstance& ClassInstanceFacade::GetClassInstance()
	{
		return *mClassInstance;
	}

	ClassInstanceFacade::operator bool() const
	{
		return mClassInstance != nullptr;
	}

	bool ClassInstanceFacade::IsSameTarget(DataTypeID aDataTypeID) const
	{
		return mClassInstance->mClass->mTargetID == aDataTypeID;
	}

	void ClassInstanceFacade::ExecuteEventInternal(const EventID aEventID, void* const aTarget, const ExecutionContextBase& aContext)
	{
		Global::GetNodeExecutor().ExecuteEvent(aEventID, *mClassInstance, aTarget, aContext);
	}

	void ClassInstanceFacade::ExecuteAutoTickersInternal(void* const aTarget, const ExecutionContextBase& aContext)
	{
		Global::GetNodeExecutor().ExecuteEvent(AutoTickEventID, *mClassInstance, aTarget, aContext);
	}

	bool operator==(const ClassInstanceFacade& a, const ClassInstanceFacade& b)
	{
		return a.mClassInstance == b.mClassInstance;
	}
}
