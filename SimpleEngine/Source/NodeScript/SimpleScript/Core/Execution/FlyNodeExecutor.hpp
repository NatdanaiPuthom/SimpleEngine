#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"
#include "FlyCallStack.hpp"
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

		/*void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);*/
		
		CallStack& GetCallStack()
		{
			return mCallStack;
		}

		void ExecuteEvent(EventID aEventID, ClassInstance& aClassInstance, void* aTarget, const ExecutionContextBase& aExecutionContext);

	private:

	private:

		//std::unordered_set<NodeExecutionData> mAutoTickNodes;

		InternalExecutionContext mExecutionContext;

		CallStack mCallStack;
	};
}