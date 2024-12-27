#include "FlyClassFacade.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Serialization/FlySerializer.hpp"

namespace FLY_NAMESPACE
{

	ClassFacade::ClassFacade(const ClassID aClassID)
		: mClassID(aClassID)
	{
		assert(&mClassID);
	}

	std::string_view ClassFacade::GetName() const
	{
		return GetClass().mName;
	}

	DataTypeFacade ClassFacade::GetTargetDataType() const
	{
		const DataTypeID targetID = GetClass().mTargetID;
		return DataTypeFacade(targetID);
	}

	NodeGraphFacade ClassFacade::GetEventGraphFacade() const
	{
		return NodeGraphFacade(GetClass().mEventGraph);
	}

	std::vector<VariableFacade> ClassFacade::GetVariables(const bool aIncludeDestroyed) const
	{
		const std::vector<Variable>& variables = GetClass().mVariableContainer.mVariables;
		std::vector<VariableFacade> variableFacades;

		variableFacades.reserve(variables.size());

		if (!aIncludeDestroyed)
		{
			for (VarID varID{ 0 }; varID < variables.size(); ++varID)
			{
				const Variable& variable = variables[varID];
				if (variable.mIsDestroyed)
				{
					continue;
				}
				variableFacades.push_back(VariableFacade(varID, *this));
			}
		}
		else
		{
			for (VarID varID{ 0 }; varID < variables.size(); ++varID)
			{
				variableFacades.push_back(VariableFacade(varID, *this));
			}
		}

		return variableFacades;
	}

	std::vector<FunctionFacade> ClassFacade::GetFunctions() const
	{
		const std::vector<FunctionID>& functionIDs = GetClass().GetMemberFunctionIDs();
		std::vector<FunctionFacade> functionFacades;

		functionFacades.reserve(functionIDs.size());

		for (const FunctionID functionID : functionIDs)
		{
			functionFacades.push_back(FunctionFacade(functionID));
		}

		return functionFacades;
	}

	VariableFacade ClassFacade::CreateVariable(const DataTypeFacade aDataTypeFacade, const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(GetClass().mVariableContainer, aDataTypeFacade.GetID(), aName, aCommandTracker);
		return VariableFacade(varID, *this);
	}

	ClassInstanceFacade ClassFacade::CreateClassInstance()
	{
		return ClassInstanceFacade(Internal::CreateClassInstance(mClassID));
	}

	FunctionFacade ClassFacade::CreateMemberFunction(const std::string_view aName)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		GetClass().BindFunction(id);

		return FunctionFacade(id);
	}

	VariableContainer& ClassFacade::GetVariableContainer() const
	{
		return GetClass().mVariableContainer;
	}

	ClassID ClassFacade::GetID() const
	{
		return mClassID;
	}

	void ClassFacade::SetName(std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetClassName(mClassID, aName, aCommandTracker);
	}

	void ClassFacade::Save(const std::string_view aSavePath) const
	{
		Internal::SaveClass(GetClass(), aSavePath);
	}

	ClassFacade::operator bool() const
	{
		return mClassID != InvalidID<ClassID>();
	}

	bool operator==(const ClassFacade& a, const ClassFacade& b)
	{
		return a.mClassID == b.mClassID;
	}

	Class& ClassFacade::GetClass() const
	{
		return Internal::GetClassByID(mClassID);
	}
}
