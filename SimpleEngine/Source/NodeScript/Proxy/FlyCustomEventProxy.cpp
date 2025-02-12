#include "FlyCustomEventProxy.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	CustomEventProxy::CustomEventProxy(const CustomEventID aID)
		: mCustomEventID(aID)
	{
	}

	NodeTypeProxy CustomEventProxy::GetCallerNodeType() const
	{
		return NodeTypeProxy(GetCustomEvent().GetCallerTypeID());
	}

	NodeTypeProxy CustomEventProxy::GetExecutorNodeType() const
	{
		return NodeTypeProxy(GetCustomEvent().GetExecutorTypeID());
	}

	CustomEventID CustomEventProxy::GetID() const
	{
		return mCustomEventID;
	}

	void CustomEventProxy::SetName(std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::SetCustomEventName(GetID(), aName, aCommandTracker);
	}

	void CustomEventProxy::AddPin(GenericDataTypeProxy aDataTypeProxy, std::string_view aName, CommandTracker* aCommandTracker)
	{
		Internal::AddPinTypeToCustomEvent(GetID(), aDataTypeProxy.GetID(), aName, aCommandTracker);
	}

	void CustomEventProxy::SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::SetPinNameAtIndexCustomEvent(GetID(), aName, aIndex, aCommandTracker);
	} 

	void CustomEventProxy::SetPinDataTypeAtIndex(GenericDataTypeProxy aDataTypeProxy, size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexCustomEvent(GetID(), aDataTypeProxy.GetID(), aIndex, aCommandTracker);
	}

	void CustomEventProxy::DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker)
	{
		Internal::DeletePinAtIndexCustomEvent(GetID(), aIndex, aCommandTracker);
	}

	CustomEventProxy::operator bool() const
	{
		return mCustomEventID != InvalidID<CustomEventID>();
	}

	const CustomEvent& CustomEventProxy::GetCustomEvent() const
	{
		return Internal::GetNodeTypeManager().GetCustomEvent(mCustomEventID);
	}

	bool operator==(const CustomEventProxy& a, const CustomEventProxy& b)
	{
		return a.mCustomEventID == b.mCustomEventID;
	}
}
