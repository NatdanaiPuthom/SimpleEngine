#pragma once
#include "../ScriptDefines.h"
#include "../Contexts/InternalExecutionContext.h"
#include "../ScriptCallStack.h"
#include <unordered_set>

namespace SCR
{

	class EventGraph;
	class ScriptInstance;

	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();

		void ExecuteEvent(EventID anEventID, ScriptInstance& aScriptInstance, void* aOwner, const ExecutionContextBase& anExecutionContext, bool aExecuteAutoTickers = false);

		template<typename EventFunction>
		void ExecuteEvent(EventFunction, ScriptInstance& aScriptInstance, void* aOwner, const ExecutionContextBase& aContext, bool aExecuteAutoTickers = false);

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

		void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);


	private:

		std::unordered_set<NodeExecutionData> myAutoTickNodes;

		InternalExecutionContext myExecutionContext;

		CallStack myCallStack;
	};

	template<typename EventFunction>
	inline void NodeExecutor::ExecuteEvent(EventFunction aFunction, ScriptInstance& aScriptInstance, void* const aOwner, const ExecutionContextBase& aContext, const bool aExecuteAutoTickers)
	{
		const EventID eventID = std::hash<EventFunction>()(aFunction);
		ExecuteEvent(eventID, aScriptInstance, aOwner, aContext, aExecuteAutoTickers);
	}
}