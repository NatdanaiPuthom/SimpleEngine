#pragma once
#include "../ScriptDefines.h"
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


		Script* myScript;
		VariableManagerInstance myVariableManagerInstance;
		NodeGraphInstance myEventGraphInstance;

	private:

	};
}