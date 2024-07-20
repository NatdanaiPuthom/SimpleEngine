#pragma once
#include "../ScriptDefines.hpp"
#include "VariableManagerInstance.hpp"
#include "NodeGraphInstance.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	struct ExecutionContextBase;

	class ClassInstance final
	{
	public:


		ClassInstance();
		~ClassInstance();

		void Init(Class& aScript);


		Class* mClass;
		VariableManagerInstance mVariableManagerInstance;
		NodeGraphInstance mEventGraphInstance;

	private:

	};
}