#include "FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"

namespace FLY_NAMESPACE
{

	ClassInstance::ClassInstance(Class& aClass)
		: mClass(&aClass)
		, mStructInstance(aClass.mStruct)
		, mEventGraphInstance(aClass.mEventGraph.mNodeGraph)
	{
	}

	ClassInstance::~ClassInstance()
	{
	}

	void ClassInstance::InitRuntime()
	{
		mStructInstance.InitRuntime();
	}
}
