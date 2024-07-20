#pragma once
#include "../ScriptDefines.hpp"
#include "../CustomEvent/CustomEvent.hpp"
#include "NodeTypeView.hpp"

namespace SCR
{
	class CustomEventView final
	{
	public:

		explicit CustomEventView(CustomEventID anID);


		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetExecutorNodeType() const;
		CustomEventID GetID() const;

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}