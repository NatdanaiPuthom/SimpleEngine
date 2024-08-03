#pragma once
#include "../FlyDefines.hpp"
#include "FlyStructInstance.hpp"
#include "FlyNodeGraphInstance.hpp"

namespace FLY_NAMESPACE
{

	class Class;

	class ClassInstance final
	{
	public:


		ClassInstance(Class& aClass);
		~ClassInstance();

		void Init();

		Class* mClass;
		StructInstance mStructInstance;
		NodeGraphInstance mEventGraphInstance;

	private:

	};
}