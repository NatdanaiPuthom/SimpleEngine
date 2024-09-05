#include "FlyCustomEventView.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{

	CustomEventView::CustomEventView(const CustomEventID aID)
		: mCustomEventID(aID)
	{
	}

	NodeTypeView CustomEventView::GetCallerNodeType() const
	{
		return NodeTypeView(GetCustomEvent().GetCallerTypeID());
	}

	NodeTypeView CustomEventView::GetExecutorNodeType() const
	{
		return NodeTypeView(GetCustomEvent().GetExecutorTypeID());
	}

	CustomEventID CustomEventView::GetID() const
	{
		return mCustomEventID;
	}

	void CustomEventView::SetName(std::string_view aName, CommandTracker* aCommandTracker)
	{
		SetCustomEventName(*this, aName, aCommandTracker);
	}

	void CustomEventView::AddPin(DataTypeView aDataTypeView, std::string_view aName, CommandTracker* aCommandTracker)
	{
		AddPinToCustomEvent(*this, aDataTypeView, aName, aCommandTracker);
	}

	void CustomEventView::SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker)
	{
		SetPinNameAtIndexCustomEvent(*this, aName, aIndex, aCommandTracker);
	} 

	void CustomEventView::SetPinDataTypeAtIndex(DataTypeView aDataTypeView, size_t aIndex, CommandTracker* aCommandTracker)
	{
		SetPinDataTypeAtIndexCustomEvent(*this, aDataTypeView, aIndex, aCommandTracker);
	}

	void CustomEventView::DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker)
	{
		DeletePinAtIndexCustomEvent(*this, aIndex, aCommandTracker);
	}

	CustomEventView::operator bool() const
	{
		return mCustomEventID != InvalidID<CustomEventID>();
	}

	const CustomEvent& CustomEventView::GetCustomEvent() const
	{
		return Global::GetNodeTypeManager().GetCustomEvent(mCustomEventID);
	}

	bool operator==(const CustomEventView& a, const CustomEventView& b)
	{
		return a.mCustomEventID == b.mCustomEventID;
	}
}
