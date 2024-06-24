#pragma once
#include "../ScriptDefines.h"
#include "../Contexts/InternalExecutionContext.h"
#include "../ScriptCallStack.h"
#include "NodeTrait.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

namespace SCR
{

	class EventGraph;
	class ScriptInstance;

	template<typename T>
	concept IsFunction = std::is_function_v<T>;


	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();

		void ExecuteEvent(const size_t anEventHash, EventGraph& anEventGraph, ScriptInstance& aScriptInstance, const ExecutionContextBase& anExecutionContext);

		template<IsFunction EventFunction>
		void ExecuteEvent(EventFunction, const ExecutionContextBase& aContext);

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

		void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);


	private:

		std::unordered_set<NodeExecutionData> myAutoTickNodes;

		InternalExecutionContext myExecutionContext;

		CallStack myCallStack;
	};

	template<IsFunction EventFunction>
	inline void NodeExecutor::ExecuteEvent(EventFunction, const ExecutionContextBase& aContext)
	{
		ExecuteEvent(typeid(EventFunction).hash_code(), aContext);
	}
}