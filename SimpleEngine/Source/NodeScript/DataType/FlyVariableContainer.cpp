#include "FlyVariableContainer.hpp"


namespace FLY_NAMESPACE
{

	VariableContainer::VariableContainer(const VariableContainer& aOther)
		: mVariables(aOther.mVariables)
		, mMemoryArena(aOther.mMemoryArena)
	{
		for (size_t i = 0; i < mVariables.size(); i++)
		{
			mVariables[i].SetDefaultValueDataPtr(mMemoryArena.GetRenewedPointer(aOther.mVariables[i].GetDefaultValueDataPtr(), aOther.mMemoryArena));
		}
	}

	VariableContainer& VariableContainer::operator=(const VariableContainer& aOther)
	{
		mVariables = aOther.mVariables;
		mMemoryArena = aOther.mMemoryArena;

		for (size_t i = 0; i < mVariables.size(); i++)
		{
			mVariables[i].SetDefaultValueDataPtr(mMemoryArena.GetRenewedPointer(aOther.mVariables[i].GetDefaultValueDataPtr(), aOther.mMemoryArena));
		}

		return *this;
	}

	void VariableContainer::AddVariable(Variable aVariable)
	{
		mVariables.push_back(std::move(aVariable));
	}
}