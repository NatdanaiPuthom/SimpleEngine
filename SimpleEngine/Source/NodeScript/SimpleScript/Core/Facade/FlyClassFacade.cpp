#include "FlyClassFacade.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{
	ClassFacade::ClassFacade()
		: mClass(nullptr)
	{
	}

	ClassFacade::ClassFacade(Class& aClass)
		: mClass(&aClass)
	{
		assert(&mClass);
	}

	const std::string& ClassFacade::GetName() const
	{
		return mClass->mName;
	}

	DataTypeFacade ClassFacade::GetTargetDataType() const
	{
		const DataTypeID targetID = mClass->mTargetID;
		return DataTypeFacade(targetID);
	}

	NodeGraphFacade ClassFacade::GetEventGraphFacade() const
	{
		return NodeGraphFacade(GetClass().mEventGraph);
	}

	std::vector<VariableFacade> ClassFacade::GetVariables(const bool aIncludeDestroyed) const
	{
		const std::vector<Variable>& variables = mClass->mStruct.mVariables;
		std::vector<VariableFacade> variableFacades;

		variableFacades.reserve(variables.size());

		if (!aIncludeDestroyed)
		{
			for (VarID varID = 0; varID < variables.size(); ++varID)
			{
				const Variable& variable = variables.at(varID);
				if (variable.mIsDestroyed)
				{
					continue;
				}
				variableFacades.push_back(VariableFacade(varID, *this));
			}
		}
		else
		{
			for (VarID varID = 0; varID < variables.size(); ++varID)
			{
				variableFacades.push_back(VariableFacade(varID, *this));
			}
		}

		return variableFacades;
	}

	std::vector<FunctionFacade> ClassFacade::GetFunctions() const
	{
		const std::vector<FunctionID>& functionIDs = mClass->GetMemberFunctionIDs();
		std::vector<FunctionFacade> functionFacades;

		functionFacades.reserve(functionIDs.size());

		for (const FunctionID functionID : functionIDs)
		{
			functionFacades.push_back(FunctionFacade(functionID));
		}

		return functionFacades;
	}

	Class& ClassFacade::GetClass() const
	{
		return *mClass;
	}

	VariableFacade ClassFacade::CreateVariable(const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		return FLY_NAMESPACE::CreateVariable(*this, aDataTypeFacade, aCommandTracker);
	}

	ClassInstanceFacade ClassFacade::CreateClassInstance()
	{
		return FLY_NAMESPACE::CreateClassInstance(*this);
	}

	FunctionFacade ClassFacade::CreateMemberFunction(const std::string_view aName)
	{
		return FLY_NAMESPACE::CreateMemberFunction(aName, *this);
	}

	bool ClassFacade::operator==(const ClassFacade& aOther) const
	{
		return mClass == aOther.mClass;
	}

	bool ClassFacade::operator!=(const ClassFacade& aOther) const
	{
		return !(*this == aOther);
	}

	ClassFacade::operator bool() const
	{
		return mClass != nullptr;
	}
}
