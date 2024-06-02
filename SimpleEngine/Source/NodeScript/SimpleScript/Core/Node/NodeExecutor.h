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

	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();

		void ExecuteEvent(const eNodeExecutionTrait aTrait, Script& aScript, const ExecutionContextBase& anExecutionContext);
		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

		void BindToEvent(const NodeRef& aNodeRef, const eNodeExecutionTrait aTrait);
		void BindToEvent(const NodeRef& aNodeRef);
		void UnbindFromEvent(const NodeRef& aNodeRef, const eNodeExecutionTrait aTrait);
		void UnbindFromEvent(const NodeRef& aNodeRef);

		void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);

	private:

		void ExecuteInternal(const std::vector<NodeExecutionData>& aNodes);


	private:

		std::unordered_map<eNodeExecutionTrait, std::vector<NodeExecutionData>> myEventNodes;
		std::unordered_set<NodeExecutionData> myAutoTickNodes;

		InternalExecutionContext myExecutionContext;

		CallStack myCallStack;
	};
}