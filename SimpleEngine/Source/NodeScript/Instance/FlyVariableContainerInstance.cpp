#include "FlyVariableContainerInstance.hpp"
#include "../DataType/FlyVariableContainer.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	VariableContainerInstance::VariableContainerInstance(const VariableContainer& aVariableContainer)
		: mVariableContainer(&aVariableContainer)
		, mVariableInstances(aVariableContainer.mVariables.size())
	{
		for (VarID varID = 0; varID < aVariableContainer.mVariables.size(); ++varID)
		{
			const Variable& variable = aVariableContainer.mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];
			variableInstance.mDefaultValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
			variableInstance.mRuntimeValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}

	VariableContainerInstance::~VariableContainerInstance()
	{
	}

	VariableContainerInstance::VariableContainerInstance(const VariableContainerInstance& aOther)
		: mVariableContainer(aOther.mVariableContainer)
		, mVariableInstances(aOther.mVariableInstances)
		, mMemoryArena(aOther.mMemoryArena)
	{
		for (VariableInstance& variableInstance : mVariableInstances)
		{
			variableInstance.mDefaultValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mDefaultValueDataPtr, aOther.mMemoryArena);
			variableInstance.mRuntimeValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeValueDataPtr, aOther.mMemoryArena);
		}
	}

	VariableContainerInstance& VariableContainerInstance::operator=(const VariableContainerInstance& aOther)
	{
		mVariableInstances = aOther.mVariableInstances;
		mMemoryArena = aOther.mMemoryArena;

		for (VariableInstance& variableInstance : mVariableInstances)
		{
			variableInstance.mDefaultValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mDefaultValueDataPtr, aOther.mMemoryArena);
			variableInstance.mRuntimeValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeValueDataPtr, aOther.mMemoryArena);
		}

		return *this;
	}

	void VariableContainerInstance::Mirror()
	{
		const size_t previousSize = mVariableInstances.size();
		mVariableInstances.resize(mVariableContainer->mVariables.size());
		for (VarID varID = previousSize; varID < mVariableContainer->mVariables.size(); ++varID)
		{
			const Variable& variable = mVariableContainer->mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];

			variableInstance.mDefaultValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
			variableInstance.mRuntimeValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}

	void VariableContainerInstance::InitRuntime()
	{
		for (VarID varID = 0; varID < mVariableContainer->mVariables.size(); ++varID)
		{
			const Variable& variable = mVariableContainer->mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];
			Global::GetDataTypeManager().CopyData(variable.mDataTypeID, variableInstance.mRuntimeValueDataPtr, variableInstance.mDefaultValueDataPtr);
		}
	}
}
