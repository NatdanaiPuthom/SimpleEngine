#pragma once
#include "../ScriptDefines.hpp"
#include "../Contexts/InternalExecutionContext.hpp"
#include "../ScriptCallStack.hpp"
#include <unordered_set>

namespace FLY_NAMESPACE
{

	class EventGraph;
	class ClassInstance;

	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();

		void ExecuteEvent(EventID anEventID, ClassInstance& aScriptInstance, void* aOwner, const ExecutionContextBase& anExecutionContext, bool aExecuteAutoTickers = false);

		template<typename EventFunction>
		void ExecuteEvent(EventFunction, ClassInstance& aScriptInstance, void* aOwner, const ExecutionContextBase& aContext, bool aExecuteAutoTickers = false);

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

		void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);
		
		CallStack& GetCallStack()
		{
			return mCallStack;
		}


	private:

		std::unordered_set<NodeExecutionData> mAutoTickNodes;

		InternalExecutionContext mExecutionContext;

		CallStack mCallStack;
	};

	template<typename EventFunction>
	inline void NodeExecutor::ExecuteEvent(EventFunction aFunction, ClassInstance& aClassInstance, void* const aOwner, const ExecutionContextBase& aContext, const bool aExecuteAutoTickers)
	{
		const EventID mEventID = std::hash<EventFunction>()(aFunction);
		ExecuteEvent(mEventID, aClassInstance, aOwner, aContext, aExecuteAutoTickers);
	}
}