#include "FlyClassInstanceProxy.hpp"
#include "../Execution/FlyNodeExecutor.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	ClassInstanceProxy::ClassInstanceProxy(ClassInstance& aClassInstance)
		: mClassInstance(&aClassInstance)
	{
	}

	void ClassInstanceProxy::InitRuntime()
	{
		mClassInstance->InitRuntime();
	}

	void ClassInstanceProxy::ViewAndEditVariableDefaultValues(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditClassInstanceVariableDefaultValue(*mClassInstance, aCommandTracker);
	}

	void ClassInstanceProxy::Destroy()
	{
		Internal::DestroyClassInstance(*mClassInstance);
	}

	std::string_view ClassInstanceProxy::GetName() const
	{
		return GetClass().mName;
	}

	ClassInstance& ClassInstanceProxy::GetClassInstance()
	{
		return *mClassInstance;
	}

	ClassInstanceProxy::operator bool() const
	{
		return mClassInstance != nullptr;
	}

	Class& ClassInstanceProxy::GetClass() const
	{
		return Internal::GetClassByID(mClassInstance->mClassID);
	}

	bool ClassInstanceProxy::IsSameTarget(DataTypeID aDataTypeID) const
	{
		return GetClass().mTargetID == aDataTypeID;
	}

	void ClassInstanceProxy::ExecuteEventInternal(const EventID aEventID, void* const aTarget, const ExecutionContextBase& aContext)
	{
		Internal::GetNodeExecutor().ExecuteEvent(aEventID, *mClassInstance, aTarget, aContext);
	}

	void ClassInstanceProxy::ExecuteAutoTickersInternal(void* const aTarget, const ExecutionContextBase& aContext)
	{
		Internal::GetNodeExecutor().ExecuteEvent(AutoTickEventID, *mClassInstance, aTarget, aContext);
	}

	bool operator==(const ClassInstanceProxy& a, const ClassInstanceProxy& b)
	{
		return a.mClassInstance == b.mClassInstance;
	}
}
