#pragma once
#include "../FlyDefines.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "FlyNodeTypeView.hpp"

namespace FLY_NAMESPACE
{
	class CustomEventView final
	{
	public:

		explicit CustomEventView(CustomEventID aID);


		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetExecutorNodeType() const;
		CustomEventID GetID() const;

		explicit operator bool() const;

		friend bool operator==(const CustomEventView& a, const CustomEventView& b);

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}