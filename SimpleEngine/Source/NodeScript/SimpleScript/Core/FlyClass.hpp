#pragma once
#include "ScriptDefines.hpp"
#include "Variable/VariableManager.hpp"
#include "ScriptNodeGraph.hpp"
#include "EventGraph.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class Function;
	class ClassInstance;

	class Class final
	{
		friend class ScriptProxy;
	public:

		Class(DataTypeID aTargetID, const std::string& aName = "Default Script");
		~Class();

		Class(const Class&) = delete;
		Class(Class&&) = delete;
		Class& operator=(const Class&) = delete;
		Class& operator=(Class&&) = delete;

		std::string& Name();
		const std::string& Name() const;

		ClassInstance& CreateClassInstance();
		void DestroyClassInstance(ClassInstance& aScriptInstance);

		EventGraph& GetEventGraph();
		DataTypeID GetTargetID() const;

		void BindFunction(FunctionID aFunctionID);
		const std::vector<FunctionID>& GetMemberFunctionIDs() const;

		const VariableManager& GetVariableManager() const;

	private:

		EventGraph mEventGraph;
		VariableManager mVariableManager;

		std::vector<FunctionID> mMemberFunctionIDs;
		std::vector<std::unique_ptr<ClassInstance>> mClassInstances;

		DataTypeID mTargetID;
		std::string mName;
	};
}

