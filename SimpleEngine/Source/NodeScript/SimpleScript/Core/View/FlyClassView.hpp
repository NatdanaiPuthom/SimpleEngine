#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableView.hpp"
#include "FlyFunctionView.hpp"
#include "FlyDataTypeView.hpp"
#include "FlyNodeGraphView.hpp"
#include "FlyClassInstanceView.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class EventGraph;

	class ClassView final
	{
	public:

		ClassView();
		explicit ClassView(Class& aClass);
		
		const std::string& GetName() const;
		DataTypeView GetTargetDataType() const;

		NodeGraphView GetEventGraphView() const;

		std::vector<VariableView> GetVariables(bool aIncludeDestroyed = false) const;
		std::vector<FunctionView> GetFunctions() const;

		Class& GetClass() const;

		VariableView CreateVariable(DataTypeView aDataTypeView, CommandTracker* aCommandTracker);
		ClassInstanceView CreateClassInstance();

		bool operator==(const ClassView& aOther) const;
		bool operator!=(const ClassView& aOther) const;

		explicit operator bool() const;

	private:

		Class* mClass;
	};
}