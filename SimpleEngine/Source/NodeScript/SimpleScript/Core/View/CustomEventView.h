#pragma once
#include "../ScriptDefines.h"
#include "../CustomEvent/CustomEvent.h"
#include "NodeTypeView.h"

namespace SCR
{
	class CustomEventView final
	{
	public:

		CustomEventView(CustomEventID anID);


		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetExecutorNodeType() const;
		CustomEventID GetID() const;

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID myCustomEventID;
	};
}