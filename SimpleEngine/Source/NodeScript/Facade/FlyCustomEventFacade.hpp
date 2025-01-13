#pragma once
#include "../FlyDefines.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "FlyNodeTypeFacade.hpp"

namespace FLY_NAMESPACE
{
	
	class DataTypeFacade;
	class GenericDataTypeFacade;
	class CommandTracker;

	class CustomEventFacade final
	{
	public:

		explicit CustomEventFacade(CustomEventID aID);


		NodeTypeFacade GetCallerNodeType() const;
		NodeTypeFacade GetExecutorNodeType() const;
		CustomEventID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(GenericDataTypeFacade aDataTypeFacade, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(GenericDataTypeFacade aDataTypeFacade, size_t aIndex, CommandTracker* aCommandTracker);
		void DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const CustomEventFacade& a, const CustomEventFacade& b);

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}