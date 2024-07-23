#include "FlyVariableManagerInstance.hpp"
#include "Variable/FlyVariableManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	VariableManagerInstance::VariableManagerInstance()
		: mMemoryArena(std::make_unique<MemoryArena<VariableArenaSize>>())
	{
	}

	VariableManagerInstance::~VariableManagerInstance()
	{
	}

	VariableManagerInstance::VariableManagerInstance(const VariableManagerInstance& aOther)
		: mVariables(aOther.mVariables)
		, mMemoryArena(std::make_unique<MemoryArena<VariableArenaSize>>(*aOther.mMemoryArena))
	{
		for (VariableInstance& variableInstance : mVariables)
		{
			variableInstance.mRuntimeDataPtr = mMemoryArena->GetRenewedPointer(variableInstance.mRuntimeDataPtr, *aOther.mMemoryArena);
		}
	}

	VariableManagerInstance& VariableManagerInstance::operator=(const VariableManagerInstance& aOther)
	{
		mVariables = aOther.mVariables;
		mMemoryArena = std::make_unique<MemoryArena<VariableArenaSize>>(*aOther.mMemoryArena);

		for (VariableInstance& variableInstance : mVariables)
		{
			variableInstance.mRuntimeDataPtr = mMemoryArena->GetRenewedPointer(variableInstance.mRuntimeDataPtr, *aOther.mMemoryArena);
		}

		return *this;
	}

	void VariableManagerInstance::Init(VariableManager& aVariableManager)
	{
		mMemoryArena->Clear();
		mVariables.resize(aVariableManager.mVariables.size());
		for (VarID varID = 0; varID < aVariableManager.mVariables.size(); ++varID)
		{
			const Variable& variable = aVariableManager.mVariables[varID];
			mVariables[varID].mRuntimeDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, *mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}
}
