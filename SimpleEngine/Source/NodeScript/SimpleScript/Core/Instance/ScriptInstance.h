#pragma once
#include "../ScriptDefines.h"
#include "NodeTrait.h"
#include "VariableManagerInstance.h"
#include "NodeGraphInstance.h"

namespace SCR
{

	class Script;
	struct ExecutionContextBase;

	class ScriptInstance final
	{
	public:


		ScriptInstance();
		~ScriptInstance();

		void Init(Script& aScript);
		void ExecuteEvent(eNodeEventType aNodeEvent, const ExecutionContextBase& aContext);


		Script* myScript;
		VariableManagerInstance myVariableManagerInstance;
		NodeGraphInstance myEventGraphInstance;

	private:

	};
}