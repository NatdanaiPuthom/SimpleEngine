#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableFacade.hpp"
#include "FlyFunctionFacade.hpp"
#include "FlyDataTypeFacade.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "FlyClassInstanceFacade.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class EventGraph;

	class ClassFacade final
	{
	public:

		ClassFacade();
		explicit ClassFacade(Class& aClass);
		
		const std::string& GetName() const;
		DataTypeFacade GetTargetDataType() const;

		NodeGraphFacade GetEventGraphFacade() const;

		std::vector<VariableFacade> GetVariables(bool aIncludeDestroyed = false) const;
		std::vector<FunctionFacade> GetFunctions() const;

		Class& GetClass() const;

		VariableFacade CreateVariable(DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);
		ClassInstanceFacade CreateClassInstance();
		FunctionFacade CreateMemberFunction(std::string_view aName);

		bool operator==(const ClassFacade& aOther) const;
		bool operator!=(const ClassFacade& aOther) const;

		explicit operator bool() const;

	private:

		Class* mClass;
	};
}