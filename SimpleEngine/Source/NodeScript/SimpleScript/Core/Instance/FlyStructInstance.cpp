#include "FlyStructInstance.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	StructInstance::StructInstance(const Struct& aStruct)
		: mStruct(&aStruct)
		, mVariableInstances(aStruct.mVariables.size())
	{
		for (VarID varID = 0; varID < aStruct.mVariables.size(); ++varID)
		{
			const Variable& variable = aStruct.mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];
			variableInstance.mDefaultValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
			variableInstance.mRuntimeValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}

	StructInstance::~StructInstance()
	{
	}

	StructInstance::StructInstance(const StructInstance& aOther)
		: mStruct(aOther.mStruct)
		, mVariableInstances(aOther.mVariableInstances)
		, mMemoryArena(aOther.mMemoryArena)
	{
		for (VariableInstance& variableInstance : mVariableInstances)
		{
			variableInstance.mDefaultValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mDefaultValueDataPtr, aOther.mMemoryArena);
			variableInstance.mRuntimeValueDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeValueDataPtr, aOther.mMemoryArena);
		}
	}

	StructInstance& StructInstance::operator=(const StructInstance& aOther)
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

	void StructInstance::Mirror()
	{
		const size_t previousSize = mVariableInstances.size();
		mVariableInstances.resize(mStruct->mVariables.size());
		for (VarID varID = previousSize; varID < mStruct->mVariables.size(); ++varID)
		{
			const Variable& variable = mStruct->mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];

			variableInstance.mDefaultValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
			variableInstance.mRuntimeValueDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}

	void StructInstance::InitRuntime()
	{
		for (VarID varID = 0; varID < mStruct->mVariables.size(); ++varID)
		{
			const Variable& variable = mStruct->mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];
			Global::GetDataTypeManager().CopyData(variable.mDataTypeID, variableInstance.mRuntimeValueDataPtr, variableInstance.mDefaultValueDataPtr);
		}
	}
}
