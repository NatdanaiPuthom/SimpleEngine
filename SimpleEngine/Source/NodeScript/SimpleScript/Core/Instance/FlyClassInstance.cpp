#include "FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"

namespace FLY_NAMESPACE
{

	ClassInstance::ClassInstance(Class& aClass)
		: mClass(&aClass)
		, mStructInstance(aClass.mStruct)
	{
	}

	ClassInstance::~ClassInstance()
	{
	}

	void ClassInstance::Init()
	{
		mStructInstance.Init();
		mEventGraphInstance.Init(mClass->mEventGraph.mNodeGraph);
	}
}
