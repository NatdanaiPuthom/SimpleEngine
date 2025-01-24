#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableProxy.hpp"
#include "FlyFunctionProxy.hpp"
#include "FlyDataTypeProxy.hpp"
#include "FlyNodeGraphProxy.hpp"
#include "FlyClassInstanceProxy.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class EventGraph;

	class ClassProxy final
	{
	public:

		ClassProxy() = default;
		explicit ClassProxy(ClassID aClassID);
		
		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] DataTypeProxy GetTargetDataType() const;

		[[nodiscard]] NodeGraphProxy GetEventGraph() const;

		[[nodiscard]] std::vector<VariableProxy> GetVariables(bool aIncludeDestroyed = false) const;
		[[nodiscard]] std::vector<FunctionProxy> GetFunctions() const;

		VariableProxy CreateVariable(GenericDataTypeProxy aDataType, std::string_view aName, CommandTracker* aCommandTracker);
		ClassInstanceProxy CreateClassInstance();
		FunctionProxy CreateMemberFunction(std::string_view aName);

		[[nodiscard]] ClassID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);

		void Save(std::string_view aSavePath) const;

		explicit operator bool() const;

		friend bool operator==(const ClassProxy& a, const ClassProxy& b);

	private:

		[[nodiscard]] Class& GetClass() const;

	private:

		ClassID mClassID = InvalidID<ClassID>();
	};
}