#pragma once
#include "../ScriptDefines.h"
#include "../Contexts/InternalExecutionContext.h"
#include "NodeTrait.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

namespace SCR
{

	class NodeExecutor
	{
	public:

		NodeExecutor(ScriptInternalModifier& aModifier);
		~NodeExecutor();

		void Execute(const std::string& aUserEventKey, const ExecutionContextBase& anExecutionContext);
		void Execute(const eNodeExecutionTrait aTrait, const ExecutionContextBase& anExecutionContext);

		void Push(const NodeExecutionData& aNodeExecutionData);
		void ForceExecuteNode(const NodeExecutionData& aNodeExecutionData);
		//void TryExecuteNode(const NodeExecutionData aNodeExecutionData);

		void Register(const NodeID aNodeID, const eNodeExecutionTrait aTrait);
		void Register(const NodeID aNodeID);
		void Unregister(const NodeID aNodeID, const eNodeExecutionTrait aTrait);
		void Unregister(const NodeID aNodeID);

		void RegisterAutoTickNode(const NodeID aNodeID);
		void UnregisterAutoTickNode(const NodeID aNodeID);

	private:

		void ExecuteInternal(const std::vector<NodeExecutionData>& aNodes, const ExecutionContextBase& anExecutionContext);

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

	private:

		using UserEventKey = std::string;
		std::unordered_map<eNodeExecutionTrait, std::vector<NodeExecutionData>> myEventNodes;
		std::unordered_map<UserEventKey, std::vector<NodeExecutionData>> myUserEventNodes;

		std::unordered_set<NodeExecutionData> myAutoTickNodes;

		std::vector<NodeExecutionData> myCurrentNodes;
		//std::unordered_set<NodeExecutionData> myFrameNodes;

		InternalExecutionContext myExecutionContext;
	};

}