#include "FlyClassInstanceFacade.hpp"
#include "../Execution/FlyNodeExecutor.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Internal/FlyInternal.hpp"

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

	void ClassInstanceFacade::ViewAndEditVariableDefaultValues(CommandTracker* const aCommandTracker)
	{
		Internal::ViewAndEditClassInstanceVariableDefaultValue(*mClassInstance, aCommandTracker);
	}

	void ClassInstanceFacade::Destroy()
	{
		Internal::DestroyClassInstance(*mClassInstance);
	}

	std::string_view ClassInstanceFacade::GetName() const
	{
		return GetClass().mName;
	}

	ClassInstance& ClassInstanceFacade::GetClassInstance()
	{
		return *mClassInstance;
	}

	ClassInstanceFacade::operator bool() const
	{
		return mClassInstance != nullptr;
	}

	Class& ClassInstanceFacade::GetClass() const
	{
		return Internal::GetClassByID(mClassInstance->mClassID);
	}

	bool ClassInstanceFacade::IsSameTarget(DataTypeID aDataTypeID) const
	{
		return GetClass().mTargetID == aDataTypeID;
	}

	void ClassInstanceFacade::ExecuteEventInternal(const EventID aEventID, void* const aTarget, const ExecutionContextBase& aContext)
	{
		Internal::GetNodeExecutor().ExecuteEvent(aEventID, *mClassInstance, aTarget, aContext);
	}

	void ClassInstanceFacade::ExecuteAutoTickersInternal(void* const aTarget, const ExecutionContextBase& aContext)
	{
		Internal::GetNodeExecutor().ExecuteEvent(AutoTickEventID, *mClassInstance, aTarget, aContext);
	}

	bool operator==(const ClassInstanceFacade& a, const ClassInstanceFacade& b)
	{
		return a.mClassInstance == b.mClassInstance;
	}
}
