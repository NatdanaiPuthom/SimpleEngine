#include "FlyCustomEventFacade.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	CustomEventFacade::CustomEventFacade(const CustomEventID aID)
		: mCustomEventID(aID)
	{
	}

	NodeTypeFacade CustomEventFacade::GetCallerNodeType() const
	{
		return NodeTypeFacade(GetCustomEvent().GetCallerTypeID());
	}

	NodeTypeFacade CustomEventFacade::GetExecutorNodeType() const
	{
		return NodeTypeFacade(GetCustomEvent().GetExecutorTypeID());
	}

	CustomEventID CustomEventFacade::GetID() const
	{
		return mCustomEventID;
	}

	void CustomEventFacade::SetName(std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::SetCustomEventName(GetID(), aName, aCommandTracker);
	}

	void CustomEventFacade::AddPin(DataTypeFacade aDataTypeFacade, std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::AddPinToCustomEvent(GetID(), aDataTypeFacade.GetID(), aName, aCommandTracker);
	}

	void CustomEventFacade::SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::SetPinNameAtIndexCustomEvent(GetID(), aName, aIndex, aCommandTracker);
	} 

	void CustomEventFacade::SetPinDataTypeAtIndex(DataTypeFacade aDataTypeFacade, size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexCustomEvent(GetID(), aDataTypeFacade.GetID(), aIndex, aCommandTracker);
	}

	void CustomEventFacade::DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::DeletePinAtIndexCustomEvent(GetID(), aIndex, aCommandTracker);
	}

	CustomEventFacade::operator bool() const
	{
		return mCustomEventID != InvalidID<CustomEventID>();
	}

	const CustomEvent& CustomEventFacade::GetCustomEvent() const
	{
		return Internal::GetNodeTypeManager().GetCustomEvent(mCustomEventID);
	}

	bool operator==(const CustomEventFacade& a, const CustomEventFacade& b)
	{
		return a.mCustomEventID == b.mCustomEventID;
	}
}
