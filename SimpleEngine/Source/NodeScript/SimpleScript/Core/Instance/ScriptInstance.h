#pragma once
#include "../ScriptDefines.h"
#include "NodeTrait.h"
#include "VariableManagerInstance.h"

namespace SCR
{

	class Script;
	struct ExecutionContextBase;

	class ScriptInstance final
	{
	public:


		ScriptInstance();
		~ScriptInstance();

		void Compile(Script& aScript);
		void ExecuteEvent(eNodeEventType aNodeEvent, const ExecutionContextBase& aContext);


		Script* myScript;
		VariableManagerInstance myVariableManagerInstance;

	private:






	};
}