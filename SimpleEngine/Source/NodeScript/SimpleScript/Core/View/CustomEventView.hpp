#pragma once
#include "../ScriptDefines.hpp"
#include "../CustomEvent/CustomEvent.hpp"
#include "NodeTypeView.hpp"

namespace FLY_NAMESPACE
{
	class CustomEventView final
	{
	public:

		explicit CustomEventView(CustomEventID anID);


		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetExecutorNodeType() const;
		CustomEventID GetID() const;

		explicit operator bool() const;

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}