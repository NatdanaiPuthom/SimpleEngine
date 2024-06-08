#pragma once
#include "Core/ScriptDefines.h"
#include "Core/Node/NodeType.h"
#include "Core/CustomEvent/CustomEvent.h"
#include "Core/Function/ScriptFunction.h"
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
		static NodeTypeID Register(NodeType&& aNodeType);

		static void SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID);
		static void SetSetterNodeTypeID(const DataTypeID aHashID, const NodeTypeID anID);

		static void SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait anOperatorTrait, const NodeTypeID anID);

		static Node CreateInstance_Getter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		static Node CreateInstance_Setter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		static Node CreateInstance_Operator(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID);

		static Node CreateInstance(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID);

		static bool CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID);

		static NodeType& GetNodeType(const NodeTypeID anID);
		static const std::vector<NodeType>& GetNodeTypes();

		static CustomEvent& GetCustomEvent(const CustomEventID anID);
		static const std::vector<CustomEvent>& GetCustomEvents();

		static CustomEventID GetCustomEventID(const NodeTypeID aNodeTypeID);

		static Function& GetFunction(const FunctionID aFunctionID);
		static const std::vector<Function*>& GetFunctions();
		static FunctionID GetFunctionID(const NodeTypeID aNodeTypeID);

		static NodeTypeID GetTypeID(const std::string& aName);

		static const std::string& GetFullName(const NodeTypeID anID);
		static std::string GetShortName(const NodeTypeID anID);
		static std::string GetNameDirectory(const NodeTypeID anID);

		static void Assert();
		static void Destroy();

	private:
		static NodeType CreateInvalidNodeType();
	private:
		static std::vector<CustomEvent> myCustomEvents;
		static std::vector<Function*> myFunctions;
		static std::vector<NodeType> myNodeTypes;

		static std::unordered_multimap<NodeTypeID, CustomEventID> myToCustomEventID;
		static std::unordered_multimap<NodeTypeID, FunctionID> myToFunctionID;
		static std::unordered_map<DataTypeID, NodeTypeID> myGetterNodeTypeIDs;
		static std::unordered_map<DataTypeID, NodeTypeID> mySetterNodeTypeIDs;
		static std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> myOperatorNodeTypeIDs;
	};
}