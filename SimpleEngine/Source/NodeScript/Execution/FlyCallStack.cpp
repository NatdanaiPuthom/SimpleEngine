#include "FlyCallStack.hpp"
#include "FlyNodeGraph.hpp"

namespace FLY_NAMESPACE
{
	CallStack::CallStack()
	{
	}

	CallStack::~CallStack()
	{
	}

	void CallStack::Push(const NodeRef& aNodeRef)
	{
		mCallStack.push(aNodeRef);
	}


	NodeRef CallStack::Pop()
	{
		NodeRef top = mCallStack.top();
		mCallStack.pop();
		return top;
	}

}

