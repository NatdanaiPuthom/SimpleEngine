#include "CustomEventView.hpp"
#include "../Node/NodeTypeManager.hpp"
#include "../Global/ScriptGlobal.hpp"

namespace FLY_NAMESPACE
{

	CustomEventView::CustomEventView(CustomEventID anID)
		: mCustomEventID(anID)
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

	const CustomEvent& CustomEventView::GetCustomEvent() const
	{
		return Global::GetNodeTypeManager().GetCustomEvent(mCustomEventID);
	}
}
