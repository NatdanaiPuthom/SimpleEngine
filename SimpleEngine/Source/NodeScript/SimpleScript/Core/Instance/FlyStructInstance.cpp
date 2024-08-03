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
			variableInstance.mRuntimeDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
			variableInstance.mDefaultDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
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
			variableInstance.mDefaultDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mDefaultDataPtr, aOther.mMemoryArena);
			variableInstance.mRuntimeDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeDataPtr, aOther.mMemoryArena);
		}
	}

	StructInstance& StructInstance::operator=(const StructInstance& aOther)
	{
		mVariableInstances = aOther.mVariableInstances;
		mMemoryArena = aOther.mMemoryArena;

		for (VariableInstance& variableInstance : mVariableInstances)
		{
			variableInstance.mDefaultDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mDefaultDataPtr, aOther.mMemoryArena);
			variableInstance.mRuntimeDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeDataPtr, aOther.mMemoryArena);
		}

		return *this;
	}

	void StructInstance::Init()
	{
		for (VarID varID = 0; varID < mStruct->mVariables.size(); ++varID)
		{
			const Variable& variable = mStruct->mVariables[varID];
			VariableInstance& variableInstance = mVariableInstances[varID];
			Global::GetDataTypeManager().CopyData(variable.mDataTypeID, variableInstance.mRuntimeDataPtr, variableInstance.mDefaultDataPtr);
		}
	}
}
