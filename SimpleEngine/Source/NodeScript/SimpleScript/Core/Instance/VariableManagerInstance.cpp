#include "VariableManagerInstance.h"
#include "Variable/VariableManager.h"
#include "../DataType/DataTypeManager.h"
#include "../Global/ScriptGlobal.h"
#include "../Memory/ScriptMemoryArena.h"

namespace SCR
{

	VariableManagerInstance::VariableManagerInstance()
		: myMemoryArena(std::make_unique<MemoryArena<VariableArenaSize>>())
	{
	}

	VariableManagerInstance::~VariableManagerInstance()
	{
	}

	VariableManagerInstance::VariableManagerInstance(const VariableManagerInstance& aOther)
		: myVariables(aOther.myVariables)
		, myMemoryArena(std::make_unique<MemoryArena<VariableArenaSize>>(*aOther.myMemoryArena))
	{
		for (VariableInstance& variableInstance : myVariables)
		{
			variableInstance.runtimeDataPtr = myMemoryArena->GetRenewedPointer(variableInstance.runtimeDataPtr, *aOther.myMemoryArena);
		}
	}

	VariableManagerInstance& VariableManagerInstance::operator=(const VariableManagerInstance& aOther)
	{
		myVariables = aOther.myVariables;
		myMemoryArena = std::make_unique<MemoryArena<VariableArenaSize>>(*aOther.myMemoryArena);

		for (VariableInstance& variableInstance : myVariables)
		{
			variableInstance.runtimeDataPtr = myMemoryArena->GetRenewedPointer(variableInstance.runtimeDataPtr, *aOther.myMemoryArena);
		}

		return *this;
	}

	void VariableManagerInstance::Init(VariableManager& aVariableManager)
	{
		myMemoryArena->Clear();
		myVariables.resize(aVariableManager.myVariables.size());
		for (VarID varID = 0; varID < aVariableManager.myVariables.size(); ++varID)
		{
			const Variable& variable = aVariableManager.myVariables[varID];
			myVariables[varID].runtimeDataPtr = Global::GetDataTypeManager().AllocateData(variable.dataTypeID, *myMemoryArena, variable.defaultValueDataPtr);
		}
	}
}
