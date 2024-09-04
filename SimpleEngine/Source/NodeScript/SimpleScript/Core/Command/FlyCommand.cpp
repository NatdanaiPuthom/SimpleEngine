#include "FlyCommand.hpp"
#include "../Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{

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
