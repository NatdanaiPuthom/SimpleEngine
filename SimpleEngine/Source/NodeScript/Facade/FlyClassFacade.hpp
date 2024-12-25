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

		ClassFacade() = default;
		explicit ClassFacade(ClassID aClassID);
		
		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] DataTypeFacade GetTargetDataType() const;

		[[nodiscard]] NodeGraphFacade GetEventGraphFacade() const;

		[[nodiscard]] std::vector<VariableFacade> GetVariables(bool aIncludeDestroyed = false) const;
		[[nodiscard]] std::vector<FunctionFacade> GetFunctions() const;

		VariableFacade CreateVariable(DataTypeFacade aDataTypeFacade, std::string_view aName, CommandTracker* aCommandTracker);
		ClassInstanceFacade CreateClassInstance();
		FunctionFacade CreateMemberFunction(std::string_view aName);

		[[nodiscard]] VariableContainer& GetVariableContainer() const;

		[[nodiscard]] ClassID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);

		void Save(std::string_view aSavePath) const;

		explicit operator bool() const;

		friend bool operator==(const ClassFacade& a, const ClassFacade& b);

	private:

		[[nodiscard]] Class& GetClass() const;

	private:

		ClassID mClassID = InvalidID<ClassID>();
	};
}