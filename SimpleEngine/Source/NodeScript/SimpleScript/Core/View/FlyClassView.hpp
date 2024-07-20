#pragma once
#include "../ScriptDefines.hpp"
#include "VariableView.hpp"
#include "FunctionView.hpp"
#include "FlyDataTypeView.hpp"

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

		EventGraph& GetEventGraph() const;

		std::vector<VariableView> GetVariables(bool aIncludeDestroyed = false) const;
		std::vector<FunctionView> GetFunctions() const;

		Class& GetClass() const;

		bool operator==(const ClassView& aOther) const;
		bool operator!=(const ClassView& aOther) const;

		operator bool() const;

	private:

		Class* mClass;
	};
}