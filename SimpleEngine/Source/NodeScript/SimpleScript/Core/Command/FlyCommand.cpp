#include "FlyCommand.hpp"
#include "../Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{
	Command::Command(const Command& aOther)
		: mName(aOther.mName)
		, mCommandFunction(aOther.mCommandFunction)
	{
	}

	Command::Command(Command&& aOther) noexcept
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
