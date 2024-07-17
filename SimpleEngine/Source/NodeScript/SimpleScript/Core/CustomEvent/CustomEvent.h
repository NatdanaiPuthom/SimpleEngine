#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	class CustomEvent final
	{
	public:

		CustomEvent(std::string_view aName);
		~CustomEvent();

		inline const NodeTypeID& GetExecutorTypeID() const
		{
			return myExecutorTypeID;
		}

		inline const NodeTypeID& GetCallerTypeID() const
		{
			return myCallerTypeID;
		}

	private:

		NodeTypeID myExecutorTypeID;
		NodeTypeID myCallerTypeID;
	};
}