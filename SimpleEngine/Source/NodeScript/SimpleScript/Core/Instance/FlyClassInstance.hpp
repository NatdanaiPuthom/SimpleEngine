#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableManagerInstance.hpp"
#include "FlyNodeGraphInstance.hpp"

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