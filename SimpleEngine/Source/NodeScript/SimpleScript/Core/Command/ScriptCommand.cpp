#include "ScriptCommand.hpp"
#include "ScriptCommandTracker.hpp"
#include "../Global/ScriptGlobal.hpp"

namespace FLY_NAMESPACE
{
	Command::Command(const Command& aOther)
		: mName(aOther.mName)
		, mCommandFunction(aOther.mCommandFunction)
	{
	}

	Command::Command(Command&& aOther)
		: mName(std::move(aOther.mName))
		, mCommandFunction(std::move(aOther.mCommandFunction))
	{
	}

	void Command::operator()(eCommandType aCommandType) const
	{
		if (Global::IsDebugging())
		{
			if (aCommandType == eCommandType::Do)
			{
				std::cout << "Do Command: " << mName << std::endl;
			}
			else
			{
				std::cout << "Undo Command: " << mName << std::endl;
			}
		}
		mCommandFunction(aCommandType);
	}

}
