#include "FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"

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
		mStructInstance.Init(aClass.mStruct);
		// TODO Fix
		mEventGraphInstance.Init(aClass.mEventGraph.mNodeGraph);
	}
}
