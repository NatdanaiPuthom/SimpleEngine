#pragma once
#include "../ScriptDefines.h"
#include "NodeType.h"
#include "../CustomEvent/CustomEvent.h"
#include <unordered_map>

namespace SCR
{

	class Script;
	class ScriptFoundation;

	class NodeTypeManager
	{
		friend class ScriptInternalModifier;
	public:
		static NodeTypeID Register(NodeType&& aNodeType);

		static void SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID);
		static void SetSetterNodeTypeID(const DataTypeID aHashID, const NodeTypeID anID);

		static void SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait anOperatorTrait, const NodeTypeID anID);

		static Node CreateInstance_Getter(const NodeID aNodeID, const DataTypeID aDataTypeID, ScriptInternalModifier& aModifier);
		static Node CreateInstance_Setter(const NodeID aNodeID, const DataTypeID aDataTypeID, ScriptInternalModifier& aModifier);
		static Node CreateInstance_Operator(const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, ScriptInternalModifier& aModifier);


		static Node CreateInstance(const NodeID aNodeID, const NodeTypeID aNodeTypeID, ScriptInternalModifier& aModifier);

		static bool CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID);

		static NodeType& GetNodeType(const NodeTypeID anID);
		static const std::vector<NodeType>& GetNodeTypes();

		static CustomEvent& GetCustomEvent(const CustomEventID anID);
		static const std::vector<CustomEvent*>& GetCustomEvents();

		static CustomEventID GetCustomEventNodeTypeID(const NodeTypeID aNodeTypeID);

		static NodeTypeID GetTypeID(const std::string& aName);

		static const std::string& GetFullName(const NodeTypeID anID);
		static std::string GetShortName(const NodeTypeID anID);
		static std::string GetNameDirectory(const NodeTypeID anID);

		static void Assert();

		static void Destroy();

	private:

		static NodeType CreateInvalidNodeType();


	private:

		inline static std::vector<NodeType> myTypes = { CreateInvalidNodeType() };
		inline static std::vector<CustomEvent*> myCustomEvents;
		inline static std::unordered_multimap<NodeTypeID, CustomEventID> myToCustomEventNodeTypeID;

		inline static std::unordered_map<DataTypeID, NodeTypeID> myGetterNodeTypeIDs;
		inline static std::unordered_map<DataTypeID, NodeTypeID> mySetterNodeTypeIDs;
		inline static std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> myOperatorNodeTypeIDs;

	};
}