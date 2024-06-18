#pragma once
#include "ScriptDefines.h"
#include "Variable/VariableManager.h"
#include "ScriptNodeGraph.h"
#include "EventGraph.h"

namespace SCR
{

	class CommandTracker;
	class ScriptManager;
	struct ExecutionContextBase;

	class ScriptInstance;

	struct MoveNodeData
	{
		ScriptVec2 startPos, endPos;
	};

	class Script final
	{
		friend class ScriptProxy;
	public:

		Script(ScriptManager& aScriptManager, const std::string& aName = "Default Script");
		~Script();

		Script(const Script&) = delete;
		Script(Script&&) = delete;
		Script& operator=(const Script&) = delete;
		Script& operator=(Script&&) = delete;

		std::string& Name();
		const std::string& Name() const;

		ScriptInstance* CreateScriptInstance();

		EventGraph& GetEventGraph();

	private:

		std::string myName;
		EventGraph myEventGraph;
		DataTypeID myTargetID;

		//NodeGraph myNodeGraph;

		VariableManager myVariableManager;

		std::vector<std::unique_ptr<ScriptInstance>> myScriptInstances;

		ScriptManager& myScriptManager;
	};
}

