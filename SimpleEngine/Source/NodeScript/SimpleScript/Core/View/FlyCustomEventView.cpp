#include "FlyCustomEventView.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"

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
