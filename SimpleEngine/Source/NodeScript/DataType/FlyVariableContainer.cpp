#include "FlyVariableContainer.hpp"


namespace FLY_NAMESPACE
{


	VariableContainer::VariableContainer(const VariableContainer& aOther)
		: mVariables(aOther.mVariables)
		, mMemoryArena(aOther.mMemoryArena)
	{
		for (size_t i = 0; i < mVariables.size(); i++)
		{
			mVariables[i].mDefaultValueDataPtr = mMemoryArena.GetRenewedPointer(aOther.mVariables[i].mDefaultValueDataPtr, aOther.mMemoryArena);
		}

	}


	VariableContainer& VariableContainer::operator=(const VariableContainer& aOther)
	{
		mVariables = aOther.mVariables;
		mMemoryArena = aOther.mMemoryArena;

		for (size_t i = 0; i < mVariables.size(); i++)
		{
			mVariables[i].mDefaultValueDataPtr = mMemoryArena.GetRenewedPointer(aOther.mVariables[i].mDefaultValueDataPtr, aOther.mMemoryArena);
		}

		return *this;
	}

}