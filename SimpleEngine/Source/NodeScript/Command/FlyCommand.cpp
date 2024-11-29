#include "FlyCommand.hpp"
#include "../Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{

	void Command::DoCommand() const
	{
		if (Global::IsDebugging())
		{
			std::cout << "Do Command: " << mName << std::endl;
		}
		mConcept->DoCommand();
	}

	void Command::UndoCommand() const
	{
		if (Global::IsDebugging())
		{
			std::cout << "Undo Command: " << mName << std::endl;
		}
		mConcept->UndoCommand();
	}
}
