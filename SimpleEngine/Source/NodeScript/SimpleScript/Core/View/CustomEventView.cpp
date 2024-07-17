#include "CustomEventView.h"
#include "../Node/NodeTypeManager.h"
#include "../Global/ScriptGlobal.h"

namespace SCR
{

	CustomEventView::CustomEventView(CustomEventID anID)
		: myCustomEventID(anID)
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
		return myCustomEventID;
	}

	const CustomEvent& CustomEventView::GetCustomEvent() const
	{
		return Global::GetNodeTypeManager().GetCustomEvent(myCustomEventID);
	}
}
