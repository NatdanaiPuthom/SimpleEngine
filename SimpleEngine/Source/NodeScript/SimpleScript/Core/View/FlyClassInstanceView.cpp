#include "FlyClassInstanceView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Execution/FlyNodeExecutor.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"

namespace FLY_NAMESPACE
{

	ClassInstanceView::ClassInstanceView(ClassInstance& aClassInstance)
		: mClassInstance(&aClassInstance)
	{
	}

	void ClassInstanceView::InitRuntime()
	{
		mClassInstance->InitRuntime();
	}

	const std::string& ClassInstanceView::GetName() const
	{
		return mClassInstance->mClass->mName;
	}

	ClassInstance& ClassInstanceView::GetClassInstance()
	{
		return *mClassInstance;
	}

	ClassInstanceView::operator bool() const
	{
		return mClassInstance != nullptr;
	}

	bool ClassInstanceView::IsSameTarget(DataTypeID aDataTypeID) const
	{
		return mClassInstance->mClass->mTargetID == aDataTypeID;
	}

	void ClassInstanceView::ExecuteEventInternal(const EventID aEventID, void* const aTarget, const ExecutionContextBase& aContext)
	{
		Global::GetNodeExecutor().ExecuteEvent(aEventID, *mClassInstance, aTarget, aContext);
	}

	void ClassInstanceView::ExecuteAutoTickersInternal(void* const aTarget, const ExecutionContextBase& aContext)
	{
		Global::GetNodeExecutor().ExecuteEvent(AutoTickEventID, *mClassInstance, aTarget, aContext);
	}

	bool operator==(const ClassInstanceView& a, const ClassInstanceView& b)
	{
		return a.mClassInstance == b.mClassInstance;
	}
}
