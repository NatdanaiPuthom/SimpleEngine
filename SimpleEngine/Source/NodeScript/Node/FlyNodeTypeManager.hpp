#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeType.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "../Function/FlyFunction.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>

namespace FLY_NAMESPACE
{
	class Class;
	class Foundation;

	class NodeTypeManager
	{
	public:

		NodeTypeManager();
		~NodeTypeManager();

		NodeTypeID Register(NodeType aNodeType);

		void SetGetterNodeTypeID(DataTypeID aDataTypeID, NodeTypeID aNodeTypeID);
		void SetSetterNodeTypeID(DataTypeID aDataTypeID, NodeTypeID aNodeTypeID);

		void SetOperatorNodeTypeID(DataTypeID aDataTypeID, eNodeOperatorTrait aOperatorTrait, NodeTypeID aNodeTypeID);
		void MapNodeTypeIDToTrait(TraitID aTraitID, DataTypeID aDataTypeID, NodeTypeID aNodeTypeID);

		Node CreateGetterNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID);
		Node CreateSetterNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID);
		Node CreateTraitNode(NodeGraph& aNodeGraph, NodeID aNodeID, TraitID aTraitID, DataTypeID aDataTypeID);
		Node CreateOperatorNode(NodeGraph& aNodeGraph, NodeID aNodeID, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID);

		Node CreateNode(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID);

		[[nodiscard]] bool CanCreateOperatorNode(eNodeOperatorTrait aTrait, DataTypeID aDataTypeID);
		[[nodiscard]] bool CanCreateNodeByTrait(TraitID aTraitID, DataTypeID aDataTypeID);

		[[nodiscard]] NodeType& GetNodeType(const NodeTypeID aNodeTypeID)
		{
			return mNodeTypes[aNodeTypeID];
		}

		[[nodiscard]] const NodeType& GetNodeType(const NodeTypeID aNodeTypeID) const
		{
			return mNodeTypes[aNodeTypeID];
		}

		[[nodiscard]] const std::vector<NodeType>& GetNodeTypes();

		[[nodiscard]] CustomEvent& GetCustomEvent(CustomEventID aID);
		[[nodiscard]] const CustomEvent& GetCustomEvent(CustomEventID aID) const;
		[[nodiscard]] std::vector<CustomEvent>& GetCustomEvents();
		[[nodiscard]] const std::vector<CustomEvent>& GetCustomEvents() const;

		[[nodiscard]] CustomEventID GetCustomEventID(NodeTypeID aNodeTypeID) const;

		[[nodiscard]] Function& GetFunction(FunctionID aFunctionID);
		[[nodiscard]] const Function& GetFunction(FunctionID aFunctionID) const;
		[[nodiscard]] const std::vector<HeapObject<Function>>& GetFunctions();
		[[nodiscard]] FunctionID GetFunctionID(NodeTypeID aNodeTypeID) const;

		[[nodiscard]] NodeTypeID GetTypeID(std::string_view aName);

		[[nodiscard]] const std::string& GetFullName(NodeTypeID aNodeTypeID) const;
		[[nodiscard]] std::string GetShortName(NodeTypeID aNodeTypeID) const;
		[[nodiscard]] std::string GetNameDirectory(NodeTypeID aNodeTypeID) const;

		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);

		[[nodiscard]] const std::unordered_map<DataTypeID, NodeTypeID>& GetTemplateMapByOperator(eNodeOperatorTrait aOperatorTrait) const;
		[[nodiscard]] const std::unordered_map<DataTypeID, NodeTypeID>& GetMapByTrait(TraitID aTraitID) const;
		[[nodiscard]] NodeTypeID GetNodeTypeIDByTraitAndDataType(TraitID aTraitID, DataTypeID aDataTypeID) const;

		void Assert();

	private:

		static NodeType CreateInvalidNodeType();

	private:
		std::vector<NodeType> mNodeTypes;
		std::vector<CustomEvent> mCustomEvents;
		std::vector<HeapObject<Function>> mFunctions;

		std::unordered_multimap<NodeTypeID, CustomEventID> mToCustomEventID;
		std::unordered_multimap<NodeTypeID, FunctionID> mToFunctionID;
		std::unordered_map<DataTypeID, NodeTypeID> mGetterNodeTypeIDs;
		std::unordered_map<DataTypeID, NodeTypeID> mSetterNodeTypeIDs;
		std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> mTemplateNodeTypeIDMap;
		std::unordered_map<TraitID, std::unordered_map<DataTypeID, NodeTypeID>> mTraitToNodeTypeIDMap;
	};
}