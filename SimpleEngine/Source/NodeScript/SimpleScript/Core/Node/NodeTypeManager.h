#pragma once
#include "Core/ScriptDefines.h"
#include "Core/Node/NodeType.h"
#include "Core/CustomEvent/CustomEvent.h"
#include "Core/Function/ScriptFunction.h"
#include "Core/Utilities/MetaScript.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace SCR
{
	class Script;
	class ScriptFoundation;
	struct NodeGraphContext;

	class NodeTypeManager
	{
		friend class ScriptProxy;
		friend class InternalModifier;
	public:

		NodeTypeManager();
		~NodeTypeManager();

		static NodeTypeManager& GetInstance();

		NodeTypeID Register(NodeType&& aNodeType);

		void SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID);
		void SetSetterNodeTypeID(const DataTypeID aHashID, const NodeTypeID anID);

		void SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait anOperatorTrait, const NodeTypeID anID);

		Node CreateInstance_Getter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		Node CreateInstance_Setter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		Node CreateInstance_Operator(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID);

		Node CreateInstance(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID);

		bool CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID);

		NodeType& GetNodeType(const NodeTypeID anID);
		const std::vector<NodeType>& GetNodeTypes();
		
		template<Predicate<const NodeType&> FilterPredicate>
		std::vector<NodeTypeID> GetNodeTypeIDsFiltered(FilterPredicate&& aPredicate);

		CustomEvent& GetCustomEvent(const CustomEventID anID);
		const std::vector<CustomEvent>& GetCustomEvents();

		CustomEventID GetCustomEventID(const NodeTypeID aNodeTypeID);

		Function& GetFunction(const FunctionID aFunctionID);
		const std::vector<std::unique_ptr<Function>>& GetFunctions();
		FunctionID GetFunctionID(const NodeTypeID aNodeTypeID);

		NodeTypeID GetTypeID(const std::string& aName);

		const std::string& GetFullName(const NodeTypeID anID);
		std::string GetShortName(const NodeTypeID anID);
		std::string GetNameDirectory(const NodeTypeID anID);

		void Assert();

	private:

		static NodeType CreateInvalidNodeType();

	private:
		std::vector<CustomEvent> myCustomEvents;
		std::vector<std::unique_ptr<Function>> myFunctions;
		std::vector<NodeType> myNodeTypes;

		std::unordered_multimap<NodeTypeID, CustomEventID> myToCustomEventID;
		std::unordered_multimap<NodeTypeID, FunctionID> myToFunctionID;
		std::unordered_map<DataTypeID, NodeTypeID> myGetterNodeTypeIDs;
		std::unordered_map<DataTypeID, NodeTypeID> mySetterNodeTypeIDs;
		std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> myOperatorNodeTypeIDs;
	};

	template<Predicate<const NodeType&> FilterPredicate>
	inline std::vector<NodeTypeID> NodeTypeManager::GetNodeTypeIDsFiltered(FilterPredicate&& aPredicate)
	{
		std::vector<NodeTypeID> nodeTypeIDs;
		nodeTypeIDs.reserve(myNodeTypes.size());

		for (NodeTypeID id = 0; const NodeType& nodeType : myNodeTypes)
		{
			if (aPredicate(nodeType))
			{
				nodeTypeIDs.push_back(id);
			}
			id++;
		}

		return nodeTypeIDs;
	}
}