#include "FlyClassView.hpp"
#include "../FlyClass.hpp"

namespace FLY_NAMESPACE
{
	ClassView::ClassView()
		: mClass(nullptr)
	{
	}

	ClassView::ClassView(Class& aClass)
		: mClass(&aClass)
	{
		assert(&mClass);
	}

	const std::string& ClassView::GetName() const
	{
		return mClass->Name();
	}

	DataTypeView ClassView::GetTargetDataType() const
	{
		const DataTypeID targetID = mClass->GetTargetID();
		return DataTypeView(targetID);
	}

	NodeGraphView ClassView::GetEventGraph() const
	{
		return NodeGraphView(GetClass().GetEventGraph());
	}

	std::vector<VariableView> ClassView::GetVariables(const bool aIncludeDestroyed) const
	{
		const std::vector<Variable>& variables = mClass->GetVariableManager().mVariables;
		std::vector<VariableView> variableViews;

		variableViews.reserve(variables.size());

		for (VarID varID = 0; varID < variables.size(); ++varID)
		{
			if (!aIncludeDestroyed)
			{
				const Variable& variable = variables.at(varID);
				if (variable.mIsDestroyed)
				{
					continue;
				}
			}
			variableViews.push_back(VariableView(varID, *mClass));
		}

		return variableViews;
	}

	std::vector<FunctionView> ClassView::GetFunctions() const
	{
		const std::vector<FunctionID>& functionIDs = mClass->GetMemberFunctionIDs();
		std::vector<FunctionView> functionViews;

		functionViews.reserve(functionIDs.size());

		for (const FunctionID functionID : functionIDs)
		{
			functionViews.push_back(FunctionView(functionID));
		}

		return functionViews;
	}

	Class& ClassView::GetClass() const
	{
		return *mClass;
	}

	bool ClassView::operator==(const ClassView& aOther) const
	{
		return mClass == aOther.mClass;
	}

	bool ClassView::operator!=(const ClassView& aOther) const
	{
		return !(*this == aOther);
	}

	ClassView::operator bool() const
	{
		return mClass != nullptr;
	}
}
