#include "FlyClassInstance.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	ClassInstance::ClassInstance(ClassID aClassID)
		: mClassID(aClassID)
		, mVariableContainerInstance(Internal::GetClassByID(aClassID).mVariableContainer)
		, mEventGraphInstance(Internal::GetClassByID(aClassID).mEventGraph.mNodeGraph)
	{
	}

	ClassInstance::~ClassInstance()
	{
	}

	void ClassInstance::InitRuntime()
	{
		mVariableContainerInstance.InitRuntime();
	}
}
