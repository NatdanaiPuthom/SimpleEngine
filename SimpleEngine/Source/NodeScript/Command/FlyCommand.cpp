#include "FlyCommand.hpp"
#include "../Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{

	void CommandNew::DoCommand() const
	{
		if (Global::IsDebugging())
		{
			std::cout << "Do Command: " << mName << std::endl;
		}
		mConcept->DoCommand();
	}

	void CommandNew::UndoCommand() const
	{
		if (Global::IsDebugging())
		{
			std::cout << "Undo Command: " << mName << std::endl;
		}
		mConcept->UndoCommand();
	}
}
