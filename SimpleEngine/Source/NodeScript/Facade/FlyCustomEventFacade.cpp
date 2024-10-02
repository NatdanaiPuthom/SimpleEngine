#include "FlyCustomEventFacade.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Fly.hpp"

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
		SetCustomEventName(*this, aName, aCommandTracker);
	}

	void CustomEventFacade::AddPin(DataTypeFacade aDataTypeFacade, std::string_view aName, CommandTracker* aCommandTracker)
	{
		AddPinToCustomEvent(*this, aDataTypeFacade, aName, aCommandTracker);
	}

	void CustomEventFacade::SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker)
	{
		SetPinNameAtIndexCustomEvent(*this, aName, aIndex, aCommandTracker);
	} 

	void CustomEventFacade::SetPinDataTypeAtIndex(DataTypeFacade aDataTypeFacade, size_t aIndex, CommandTracker* aCommandTracker)
	{
		SetPinDataTypeAtIndexCustomEvent(*this, aDataTypeFacade, aIndex, aCommandTracker);
	}

	void CustomEventFacade::DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker)
	{
		DeletePinAtIndexCustomEvent(*this, aIndex, aCommandTracker);
	}

	CustomEventFacade::operator bool() const
	{
		return mCustomEventID != InvalidID<CustomEventID>();
	}

	const CustomEvent& CustomEventFacade::GetCustomEvent() const
	{
		return Global::GetNodeTypeManager().GetCustomEvent(mCustomEventID);
	}

	bool operator==(const CustomEventFacade& a, const CustomEventFacade& b)
	{
		return a.mCustomEventID == b.mCustomEventID;
	}
}
