#pragma once
#include "../FlyDefines.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "FlyNodeTypeView.hpp"

namespace FLY_NAMESPACE
{
	
	class DataTypeView;
	class CommandTracker;

	class CustomEventView final
	{
	public:

		explicit CustomEventView(CustomEventID aID);


		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetExecutorNodeType() const;
		CustomEventID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(DataTypeView aDataTypeView, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(DataTypeView aDataTypeView, size_t aIndex, CommandTracker* aCommandTracker);
		void DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const CustomEventView& a, const CustomEventView& b);

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}