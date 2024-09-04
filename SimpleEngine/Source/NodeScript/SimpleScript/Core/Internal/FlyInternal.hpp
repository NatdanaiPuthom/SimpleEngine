#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinType.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "../SystemTypes/FlyVec2.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../Variable/FlyVariableRef.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include <string>
#include <variant>

namespace FLY_NAMESPACE
{

	class Class;
	class CommandTracker;
	class EventGraph;


	struct NodeGraphContext
	{
		NodeGraph& mNodeGraph;
		Class& script;
	};

	namespace Internal
	{


		Class& CreateClass(DataTypeID aTargetID, std::string_view aName);

		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);
		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, std::string_view aName, bool& aSuccess, Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		NodeID CreateGetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateSetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, NodeID aNodeID, CommandTracker* aCommandTracker);

		void DestroyNode(NodeGraph& aNodeGraph, NodeID aNodeID, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<GlobalNodeRef>& aNodeRefs, CommandTracker* aCommandTracker);

		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);


		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID, size_t aStartIndex = 0);
		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, NodeID, NodeTypeID aNodeTypeID, size_t aStartIndex);

		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID);
		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID, void* aDataPtr);

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker);
		LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);
		void DestroyLinksByPin(NodeGraph& aNodeGraph, PinID aPinID, CommandTracker* aCommandTracker);

		VarID CreateVariable(Class& aClass, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableDataType(Class& aClass, VarID aVarID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableName(Class& aClass, VarID aVarID, std::string_view aName, CommandTracker* aCommandTracker);
		void DestroyVariableNodes(Class& aClass, VarID aVarID, CommandTracker* aCommandTracker);

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);

		void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName);

		void AddPinToCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, std::string_view aPinName);
		void SetPinDataTypeAtIndexCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, size_t aIndex);
		void SetPinNameAtIndexCustomEvent(CustomEventID aCustomEventID, std::string_view aName, size_t aIndex);
		void DeletePinAtIndexCustomEvent(CustomEventID aCustomEventID, size_t aIndex);

		void AddPinToFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName = "Pin");
		void SetPinDataTypeAtIndexFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, size_t aIndex, eFlowType aFlowType);
		void SetPinNameAtIndexFunction(FunctionID aFunctionID, std::string_view aName, size_t aIndex, eFlowType aFlowType);
		void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t aIndex, eFlowType aFlowType);

		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, PinID aWildcardPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);

		std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
		std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef);
		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef);
	}



}