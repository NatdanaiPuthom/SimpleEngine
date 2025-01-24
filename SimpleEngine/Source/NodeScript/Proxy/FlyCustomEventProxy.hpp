#pragma once
#include "../FlyDefines.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "FlyNodeTypeProxy.hpp"

namespace FLY_NAMESPACE
{
	
	class DataTypeProxy;
	class GenericDataTypeProxy;
	class CommandTracker;

	class CustomEventProxy final
	{
	public:

		explicit CustomEventProxy(CustomEventID aID);


		NodeTypeProxy GetCallerNodeType() const;
		NodeTypeProxy GetExecutorNodeType() const;
		CustomEventID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(GenericDataTypeProxy aDataTypeProxy, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(GenericDataTypeProxy aDataTypeProxy, size_t aIndex, CommandTracker* aCommandTracker);
		void DeletePinAtIndex(size_t aIndex, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const CustomEventProxy& a, const CustomEventProxy& b);

	private:

		const CustomEvent& GetCustomEvent() const;


	private:

		CustomEventID mCustomEventID;
	};
}