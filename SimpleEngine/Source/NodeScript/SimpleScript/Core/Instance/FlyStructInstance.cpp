#include "FlyStructInstance.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	StructInstance::StructInstance()
	{
	}

	StructInstance::~StructInstance()
	{
	}

	StructInstance::StructInstance(const StructInstance& aOther)
		: mVariables(aOther.mVariables)
		, mMemoryArena(aOther.mMemoryArena)
	{
		for (VariableInstance& variableInstance : mVariables)
		{
			variableInstance.mRuntimeDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeDataPtr, aOther.mMemoryArena);
		}
	}

	StructInstance& StructInstance::operator=(const StructInstance& aOther)
	{
		mVariables = aOther.mVariables;
		mMemoryArena = aOther.mMemoryArena;

		for (VariableInstance& variableInstance : mVariables)
		{
			variableInstance.mRuntimeDataPtr = mMemoryArena.GetRenewedPointer(variableInstance.mRuntimeDataPtr, aOther.mMemoryArena);
		}

		return *this;
	}

	void StructInstance::Init(Struct& aStruct)
	{
		mMemoryArena.Clear();
		mVariables.resize(aStruct.mVariables.size());
		for (VarID varID = 0; varID < aStruct.mVariables.size(); ++varID)
		{
			const Variable& variable = aStruct.mVariables[varID];
			mVariables[varID].mRuntimeDataPtr = Global::GetDataTypeManager().AllocateData(variable.mDataTypeID, mMemoryArena, variable.mDefaultValueDataPtr);
		}
	}
}
