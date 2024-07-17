#pragma once
#include "ScriptDefines.h"
#include "Variable/VariableManager.h"
#include "ScriptNodeGraph.h"
#include "EventGraph.h"

namespace SCR
{

	class CommandTracker;
	class Function;
	class ScriptInstance;

	class Script final
	{
		friend class ScriptProxy;
	public:

		Script(DataTypeID aTargetID, const std::string& aName = "Default Script");
		~Script();

		Script(const Script&) = delete;
		Script(Script&&) = delete;
		Script& operator=(const Script&) = delete;
		Script& operator=(Script&&) = delete;

		std::string& Name();
		const std::string& Name() const;

		ScriptInstance& CreateScriptInstance();
		void DestroyScriptInstance(ScriptInstance& aScriptInstance);

		EventGraph& GetEventGraph();
		DataTypeID GetTargetID() const;

		void BindFunction(FunctionID aFunctionID);
		const std::vector<FunctionID>& GetMemberFunctionIDs() const;

		const VariableManager& GetVariableManager() const;

	private:

		EventGraph myEventGraph;
		VariableManager myVariableManager;

		std::vector<FunctionID> myMemberFunctionIDs;
		std::vector<std::unique_ptr<ScriptInstance>> myScriptInstances;

		DataTypeID myTargetID;
		std::string myName;
	};
}

