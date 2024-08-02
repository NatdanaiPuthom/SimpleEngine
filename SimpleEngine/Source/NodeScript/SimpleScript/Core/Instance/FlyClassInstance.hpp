#pragma once
#include "../FlyDefines.hpp"
#include "FlyStructInstance.hpp"
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
		StructInstance mStructInstance;
		NodeGraphInstance mEventGraphInstance;

	private:

	};
}