#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinType.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "../SystemTypes/FlyVec2.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../Variable/FlyVariableRef.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Node/NodeDragData.hpp"
#include <string>
#include <variant>
#include <unordered_map>

namespace FLY_NAMESPACE
{

	class Class;
	class CommandTracker;
	class EventGraph;
	struct NodeType;
	class ClassInstance;

	namespace Internal
	{
		const Pin& GetPin(PinID aPinID, const NodeGraph& aNodeGraph);
		Pin& GetPin(PinID aPinID, NodeGraph& aNodeGraph);
		const PinType& GetPinType(const Pin& aPin);
		Node& GetNode(NodeID aNodeID, NodeGraph& aNodeGraph);
		const Node& GetNode(NodeID aNodeID, const NodeGraph& aNodeGraph);
		const NodeType& GetNodeType(NodeID aNodeID, const NodeGraph& aNodeGraph);

		Class& CreateClass(DataTypeID aTargetID, std::string_view aName);

		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);
		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, std::string_view aName, bool& aSuccess, Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		NodeID CreateNodeAutoLink(const NodeGraphVariant& aNodeGraphVariant, NodeTypeID aNodeTypeID, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateGetterNode(NodeGraph& aNodeGraph, VarID aVarID, Class& aClass, DataTypeID aDataTypeID, Vec2 aPosition, CommandTracker* aCommandTracker);
		NodeID CreateSetterNode(NodeGraph& aNodeGraph, VarID aVarID, Class& aClass, DataTypeID aDataTypeID, Vec2 aPosition, CommandTracker* aCommandTracker);
		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, NodeID aNodeID, CommandTracker* aCommandTracker);

		void DestroyNode(NodeGraph& aNodeGraph, NodeID aNodeID, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<GlobalNodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<NodeID>& aNodeIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestroyLinks(const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker);

		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID, size_t aStartIndex = 0);
		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, NodeID, NodeTypeID aNodeTypeID, size_t aStartIndex);

		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID);
		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID, void* aDataPtr);

		void ViewAndEditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void ViewPin(PinID aPinID, const NodeGraph& aNodeGraph);
		void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void BeginFrame(CommandTracker* aCommandTracker);

		bool IsNodeReplacable(NodeGraph& aNodeGraph, NodeID aNodeID);

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker);
		LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);
		void DestroyLinksByPin(NodeGraph& aNodeGraph, PinID aPinID, CommandTracker* aCommandTracker);

		VarID CreateVariable(Class& aClass, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableDataType(VarID aVarID, Class& aClass, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableName(VarID aVarID, Class& aClass, std::string_view aName, CommandTracker* aCommandTracker);
		void DestroyVariable(VarID aVarID, Class& aClass, CommandTracker* aCommandTracker);
		void DestroyVariableNodes(VarID aVarID, Class& aClass, CommandTracker* aCommandTracker);
		void EditVariableDefaultValue(VarID aVarID, Class& aClass, CommandTracker* aCommandTracker);

		void EditClassInstanceVariableDefaultValue(ClassInstance& aClassInstance, CommandTracker* aCommandTracker);

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);

		void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName);

		void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName, CommandTracker* aCommandTracker);
		void AddPinToCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, std::string_view aPinName, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndexCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, size_t aIndex, CommandTracker* aCommandTracker);
		void SetPinNameAtIndexCustomEvent(CustomEventID aCustomEventID, std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
		void DeletePinAtIndexCustomEvent(CustomEventID aCustomEventID, size_t aIndex, CommandTracker* aCommandTracker);

		void AddPinToFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndexFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void SetPinNameAtIndexFunction(FunctionID aFunctionID, std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, PinID aWildcardPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);

		std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
		std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef);
		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef);


		PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType);
		size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID);
		PinID GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID);

		Link ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2);

		LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed = false);
		std::vector<LinkID> GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed = false);
		std::vector<LinkID> GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID);

	}



}