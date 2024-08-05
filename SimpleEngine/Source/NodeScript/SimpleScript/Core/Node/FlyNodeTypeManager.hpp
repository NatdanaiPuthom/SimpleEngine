#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeType.hpp"
#include "../View/FlyNodeTypeView.hpp"
#include "../CustomEvent/FlyCustomEvent.hpp"
#include "../Function/FlyFunction.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace FLY_NAMESPACE
{
	class Class;
	class Foundation;

	class NodeTypeManager
	{
	public:

		NodeTypeManager();
		~NodeTypeManager();

		NodeTypeID Register(NodeType&& aNodeType);

		void SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID);
		void SetSetterNodeTypeID(const DataTypeID aHashID, const NodeTypeID anID);

		void SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait anOperatorTrait, const NodeTypeID anID);

		Node CreateGetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		Node CreateSetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID);
		Node CreateOperatorNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID);

		Node CreateNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID);

		bool CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID);

		NodeType& GetNodeType(NodeTypeID aID);
		const NodeType& GetNodeType(NodeTypeID aID) const;

		const std::vector<NodeType>& GetNodeTypes();
		
		template<Predicate<const NodeType&> FilterPredicate>
		std::vector<NodeTypeView> GetNodeTypesFiltered(FilterPredicate&& aPredicate);

		CustomEvent& GetCustomEvent(CustomEventID aID);
		const CustomEvent& GetCustomEvent(CustomEventID aID) const;
		std::vector<CustomEvent>& GetCustomEvents();
		const std::vector<CustomEvent>& GetCustomEvents() const;

		CustomEventID GetCustomEventID(NodeTypeID aNodeTypeID) const;

		Function& GetFunction(FunctionID aFunctionID);
		const Function& GetFunction(FunctionID aFunctionID) const;
		const std::vector<std::unique_ptr<Function>>& GetFunctions();
		FunctionID GetFunctionID(NodeTypeID aNodeTypeID) const;

		NodeTypeID GetTypeID(std::string_view aName);

		const std::string& GetFullName(NodeTypeID anID) const;
		std::string GetShortName(NodeTypeID anID) const;
		std::string GetNameDirectory(NodeTypeID anID) const;

		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);

		const std::unordered_map<DataTypeID, NodeTypeID>& GetTemplateMapByOperator(eNodeOperatorTrait aOperatorTrait) const;

		void Assert();

	private:

		static NodeType CreateInvalidNodeType();

	private:
		std::vector<NodeType> mNodeTypes;
		std::vector<CustomEvent> mCustomEvents;
		std::vector<std::unique_ptr<Function>> mFunctions;

		std::unordered_multimap<NodeTypeID, CustomEventID> mToCustomEventID;
		std::unordered_multimap<NodeTypeID, FunctionID> mToFunctionID;
		std::unordered_map<DataTypeID, NodeTypeID> mGetterNodeTypeIDs;
		std::unordered_map<DataTypeID, NodeTypeID> mSetterNodeTypeIDs;
		std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> mTemplateNodeTypeIDMap;
	};

	template<Predicate<const NodeType&> FilterPredicate>
	inline std::vector<NodeTypeView> NodeTypeManager::GetNodeTypesFiltered(FilterPredicate&& aPredicate)
	{
		std::vector<NodeTypeView> nodeTypes;
		nodeTypes.reserve(mNodeTypes.size());

		for (NodeTypeID nodeTypeID = 0; nodeTypeID < mNodeTypes.size(); ++nodeTypeID)
		{
			const NodeType& nodeType = mNodeTypes[nodeTypeID];
			if (aPredicate(nodeType))
			{
				nodeTypes.push_back(NodeTypeView(nodeTypeID));
			}
		}

		return nodeTypes;
	}
}