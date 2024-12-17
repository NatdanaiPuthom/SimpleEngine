#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"
#include "FlyCallStack.hpp"
#include "FlyDebugger.hpp"
#include <unordered_set>

namespace FLY_NAMESPACE
{

	class ClassInstance;

	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);
		
		CallStack& GetCallStack()
		{
			return mCallStack;
		}

		void ExecuteEvent(EventID aEventID, ClassInstance& aClassInstance, void* aTarget, const ExecutionContextBase& aExecutionContext);


		Debugger& GetDebugger();
	

	private:

		InternalExecutionContext mExecutionContext;

		CallStack mCallStack;

		Debugger mDebugger;
		
	};
}