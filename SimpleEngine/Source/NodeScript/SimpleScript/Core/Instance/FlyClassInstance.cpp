#include "FlyClassInstance.hpp"
#include "../FlyClass.hpp"
#include "ScriptProxy.hpp"

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
		mVariableManagerInstance.Init(ScriptProxy::GetVariableManager(aClass));
		// TODO Fix
		mEventGraphInstance.Init(aClass.GetEventGraph().mNodeGraph);
	}
}
