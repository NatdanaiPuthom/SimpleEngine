#include "FlyClassInstance.hpp"
#include "../FlyClass.hpp"

namespace FLY_NAMESPACE
{

	ClassInstance::ClassInstance()
		: mClass(nullptr)
	{
	}

	ClassInstance::~ClassInstance()
	{
	}

	void ClassInstance::Init(Class& aClass)
	{
		mClass = &aClass;
		mVariableManagerInstance.Init(aClass.mVariableManager);
		// TODO Fix
		mEventGraphInstance.Init(aClass.mEventGraph.mNodeGraph);
	}
}
