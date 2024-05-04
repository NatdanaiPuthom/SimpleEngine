#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{

	class MemoryPool;
	
	class CustomEvent final
	{
	public:

		CustomEvent(const std::string& aName);
		~CustomEvent();

	public:

		NodeTypeID myExecutorTypeID;
		NodeTypeID myCallerTypeID;
	};
}