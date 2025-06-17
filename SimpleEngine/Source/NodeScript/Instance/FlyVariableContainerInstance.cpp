#include "FlyVariableContainerInstance.hpp"
#include "../DataType/FlyVariableContainer.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	VariableContainerInstance::VariableContainerInstance(const VariableContainer& aVariableContainer)
		: mVariableContainer(&aVariableContainer)
		, mVariableInstances(aVariableContainer.GetVariableCount())
	{
		for (VarID varID{ 0 }; varID < aVariableContainer.GetVariableCount(); ++varID)
		{
			const Variable& variable = aVariableContainer.GetVariable(varID);
			VariableInstance& variableInstance = mVariableInstances[varID];
			variableInstance.mDefaultValueDataPtr = Internal::GetDataTypeManager().AllocateData(variable.GetDataTypeID(), mMemoryArena, variable.GetDefaultValueDataPtr());
			variableInstance.mRuntimeValueDataPtr = Internal::GetDataTypeManager().AllocateData(variable.GetDataTypeID(), mMemoryArena, variable.GetDefaultValueDataPtr());
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
		mVariableInstances.resize(mVariableContainer->GetVariableCount());
		for (VarID varID{ previousSize }; varID < mVariableContainer->GetVariableCount(); ++varID)
		{
			const Variable& variable = mVariableContainer->GetVariable(varID);
			VariableInstance& variableInstance = mVariableInstances[varID];

			variableInstance.mDefaultValueDataPtr = Internal::GetDataTypeManager().AllocateData(variable.GetDataTypeID(), mMemoryArena, variable.GetDefaultValueDataPtr());
			variableInstance.mRuntimeValueDataPtr = Internal::GetDataTypeManager().AllocateData(variable.GetDataTypeID(), mMemoryArena, variable.GetDefaultValueDataPtr());
		}
	}

	void VariableContainerInstance::InitRuntime()
	{
		for (VarID varID{ 0 }; varID < mVariableContainer->GetVariableCount(); ++varID)
		{
			const Variable& variable = mVariableContainer->GetVariable(varID);
			VariableInstance& variableInstance = mVariableInstances[varID];
			Internal::GetDataTypeManager().CopyData(variable.GetDataTypeID(), variableInstance.mRuntimeValueDataPtr, variableInstance.mDefaultValueDataPtr);
		}
	}

	bool operator==(const VariableContainerInstance& a, const VariableContainerInstance& b)
	{
		if (a.mVariableContainer == nullptr && b.mVariableContainer == nullptr)
		{
			return true;
		}

		if (a.mVariableInstances.size() != b.mVariableInstances.size())
		{
			return false;
		}

		for (size_t i = 0; i < a.mVariableInstances.size(); i++)
		{
			const VariableInstance& variableInstanceA = a.mVariableInstances[i];
			const VariableInstance& variableInstanceB = b.mVariableInstances[i];

			const Variable& variableA = a.mVariableContainer->GetVariable(i);
			const Variable& variableB = b.mVariableContainer->GetVariable(i);

			if (variableA.GetDataTypeID() != variableB.GetDataTypeID())
			{
				return false;
			}

			if (!Internal::GetDataTypeManager().DataEqualsTo(variableA.GetDataTypeID(), variableInstanceA.mDefaultValueDataPtr, variableInstanceB.mDefaultValueDataPtr))
			{
				return false;
			}

		}

		return false;
	}
}
