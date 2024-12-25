#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableContainerInstance.hpp"
#include "FlyNodeGraphInstance.hpp"

namespace FLY_NAMESPACE
{

	class Class;

	class ClassInstance final
	{
	public:


		ClassInstance(ClassID aClassID);
		~ClassInstance();

		void InitRuntime();

		ClassID mClassID;
		VariableContainerInstance mVariableContainerInstance;
		NodeGraphInstance mEventGraphInstance;

	private:

	};
}