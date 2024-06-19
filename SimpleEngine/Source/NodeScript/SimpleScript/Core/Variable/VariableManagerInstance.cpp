#include "VariableManagerInstance.h"
#include "Variable/VariableManager.h"
#include "DataTypeManager.h"
#include "../Global/ScriptGlobal.h"

namespace SCR
{

	VariableManagerInstance::VariableManagerInstance()
		: myMemoryManager(std::make_unique<MemoryManager>())
	{
	}

	VariableManagerInstance::~VariableManagerInstance()
	{
	}

	VariableManagerInstance::VariableManagerInstance(const VariableManagerInstance& aOther)
		: myVariables(aOther.myVariables)
		, myMemoryManager(std::make_unique<MemoryManager>(*aOther.myMemoryManager))
	{
		for (VariableInstance& variableInstance : myVariables)
		{
			variableInstance.runtimeDataPtr = myMemoryManager->GetMemory().GetRenewedPointer(variableInstance.runtimeDataPtr, aOther.myMemoryManager->GetMemory());
		}
	}

	VariableManagerInstance& VariableManagerInstance::operator=(const VariableManagerInstance& aOther)
	{
		myVariables = aOther.myVariables;
		myMemoryManager = std::make_unique<MemoryManager>(*aOther.myMemoryManager);

		for (VariableInstance& variableInstance : myVariables)
		{
			variableInstance.runtimeDataPtr = myMemoryManager->GetMemory().GetRenewedPointer(variableInstance.runtimeDataPtr, aOther.myMemoryManager->GetMemory());
		}

		return *this;
	}

	void VariableManagerInstance::Compile(VariableManager& aVariableManager)
	{
		myMemoryManager->Clear();
		myVariables.resize(aVariableManager.myVariables.size());
		for (size_t i = 0; const Variable& variable : aVariableManager.myVariables)
		{
			myVariables[i].runtimeDataPtr = Global::GetDataTypeManager().AllocateData(variable.dataTypeID, *myMemoryManager, variable.defaultValueDataPtr);
			++i;
		}
	}
}
