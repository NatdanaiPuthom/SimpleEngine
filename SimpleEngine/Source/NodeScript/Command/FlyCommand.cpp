#include "FlyCommand.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	void Command::DoCommand() const
	{
		if (Internal::IsDebugging())
		{
			std::cout << "Do Command: " << mName << std::endl;
		}
		mConcept->DoCommand();
	}

	void Command::UndoCommand() const
	{
		if (Internal::IsDebugging())
		{
			std::cout << "Undo Command: " << mName << std::endl;
		}
		mConcept->UndoCommand();
	}
}
