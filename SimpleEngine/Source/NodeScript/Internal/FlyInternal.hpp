#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinType.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "../SystemTypes/FlyVec2.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../DataType/FlyVariableRef.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Node/FlyNodeDragData.hpp"
#include <string>
#include <variant>
#include <unordered_map>

namespace FLY_NAMESPACE
{

	class Struct;
	class Class;
	class CommandTracker;
	class EventGraph;
	struct NodeType;
	class StructInstance;
	class ClassInstance;
	class VariableContainer;
	class Foundation;
	class NodeExecutor;
	class DataTypeManager;
	class NodeTypeManager;
	class PinTypeManager;
	class TraitManager;
	struct DataType;
	class Trait;
	
	template<size_t> class MemoryArena;
	class MemoryPool;

	namespace Internal
	{


		[[nodiscard]] Foundation& GetFoundation();
		[[nodiscard]] NodeExecutor& GetNodeExecutor();
		[[nodiscard]] DataTypeManager& GetDataTypeManager();
		[[nodiscard]] NodeTypeManager& GetNodeTypeManager();
		[[nodiscard]] PinTypeManager& GetPinTypeManager();
		[[nodiscard]] TraitManager& GetTraitManager();

		[[nodiscard]] bool& IsDebugging();

		[[nodiscard]] MemoryArena<1024>& GetFrameMemoryArena();
		[[nodiscard]] MemoryArena<10000>& GetEditMemoryArena();
		[[nodiscard]] MemoryPool& GetMemoryPool();
		[[nodiscard]] EventGraph& GetNodeGraphCopy();

		[[nodiscard]] NodeGraph& GetNodeGraph(const NodeGraphVariantHandle& aNodeGraphVariantHandle);
		[[nodiscard]] const Pin& GetPin(PinID aPinID, const NodeGraph& aNodeGraph);
		[[nodiscard]] Pin& GetPin(PinID aPinID, NodeGraph& aNodeGraph);
		[[nodiscard]] const PinType& GetPinType(PinID aPinID, const NodeGraph& aNodeGraph);
		[[nodiscard]] const PinType& GetPinType(const Pin& aPin);
		[[nodiscard]] const PinType& GetPinType(PinTypeID aPinTypeID);
		[[nodiscard]] Node& GetNode(NodeID aNodeID, NodeGraph& aNodeGraph);
		[[nodiscard]] const Node& GetNode(NodeID aNodeID, const NodeGraph& aNodeGraph);
		[[nodiscard]] const NodeType& GetNodeType(const Node& aNode);
		[[nodiscard]] const NodeType& GetNodeType(NodeID aNodeID, const NodeGraph& aNodeGraph);
		[[nodiscard]] NodeType& GetNodeType(NodeTypeID aNodeTypeID);
		[[nodiscard]] const DataType* GetDataTypeByID(DataTypeID aDataTypeID);
		[[nodiscard]] const DataType* GetDataTypeByID(GenericDataTypeID aDataTypeID);
		[[nodiscard]] Class& GetClassByID(ClassID aClassID);
		[[nodiscard]] Trait& GetTraitByID(TraitID aTraitID);

		void InitializeSubPins();

		DataTypeID CreateStruct(std::string_view aName);
		//void SetStructName(StructID aStructID, std::string_view aName, CommandTracker* aCommandTracker);

		ClassID CreateClass(GenericDataTypeID aTargetID, std::string_view aName);
		void SetClassName(ClassID aClassID, std::string_view aName, CommandTracker* aCommandTracker);
		ClassInstance& CreateClassInstance(ClassID aClassID);
		void DestroyClassInstance(ClassInstance& aClassInstance);

		TraitID CreateTrait(std::string_view aName);
		void CreateTraitImplementation(DataTypeID aDataTypeID, TraitID aTraitID);

		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);
		NodeID CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, std::string_view aName, bool& aSuccess, Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		NodeID CreateNodeAutoLink(const NodeGraphVariantHandle& aNodeGraphVariant, NodeTypeID aNodeTypeID, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
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

		void ViewAndEditPinGeneric(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		//void ViewAndEditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void ViewPinGeneric(PinID aPinID, const NodeGraph& aNodeGraph);
		//void ViewPin(PinID aPinID, const NodeGraph& aNodeGraph);
		void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void RecombinePin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void BeginFrame(CommandTracker* aCommandTracker);

		bool IsNodeReplacable(NodeGraph& aNodeGraph, NodeID aNodeID);

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker);
		LinkID CreateLinkGeneric(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);
		void DestroyLinksByPin(NodeGraph& aNodeGraph, PinID aPinID, CommandTracker* aCommandTracker);

		VarID CreateVariable(GenericDataTypeID aParentDataTypeID, GenericDataTypeID aDataTypeID, std::string_view aName, CommandTracker* aCommandTracker);
		VarID CreateVariable(VariableContainer& aVariableContainer, GenericDataTypeID aDataTypeID, std::string_view aName, CommandTracker* aCommandTracker);
		void SetVariableDataType(VarID aVarID, VariableContainer& aVariableContainer, GenericDataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableName(VarID aVarID, VariableContainer& aVariableContainer, std::string_view aName, CommandTracker* aCommandTracker);
		void DestroyVariable(VarID aVarID, VariableContainer& aVariableContainer, CommandTracker* aCommandTracker);
		//void DestroyVariableNodes(VarID aVarID, VariableContainer& aVariableContainer, CommandTracker* aCommandTracker);
		void ViewAndEditVariableDefaultValue(VarID aVarID, VariableContainer& aVariableContainer, CommandTracker* aCommandTracker);

		void ViewAndEditClassInstanceVariableDefaultValue(ClassInstance& aClassInstance, CommandTracker* aCommandTracker);

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);

		void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName);
		PinTypeID AddPinTypeToNodeType(NodeTypeID aNodeTypeID, GenericDataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName);

		void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName, CommandTracker* aCommandTracker);
		void AddPinTypeToCustomEvent(CustomEventID aCustomEventID, GenericDataTypeID aDataTypeID, std::string_view aPinName, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndexCustomEvent(CustomEventID aCustomEventID, GenericDataTypeID aDataTypeID, size_t aIndex, CommandTracker* aCommandTracker);
		void SetPinNameAtIndexCustomEvent(CustomEventID aCustomEventID, std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
		void DeletePinAtIndexCustomEvent(CustomEventID aCustomEventID, size_t aIndex, CommandTracker* aCommandTracker);

		void AddPinTypeToFunction(FunctionID aFunctionID, GenericDataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndexFunction(FunctionID aFunctionID, GenericDataTypeID aDataTypeID, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void SetPinNameAtIndexFunction(FunctionID aFunctionID, std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

		void ReplaceNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, PinID aWildcardPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(NodeGraph& aNodeGraph, NodeID aNodeID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		[[nodiscard]] NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);

		[[nodiscard]] std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
		[[nodiscard]] std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

		[[nodiscard]] VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef);
		[[nodiscard]] std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef);


		[[nodiscard]] PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType);
		[[nodiscard]] size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID);
		[[nodiscard]] PinID GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID);

		[[nodiscard]] bool AreDataTypesLinkable(GenericDataTypeID aInputDataTypeID, GenericDataTypeID aOutputDataTypeID);
		[[nodiscard]] bool AreDataTypesLinkable(DataTypeID aInputDataTypeID, DataTypeID aOutputDataTypeID);
		[[nodiscard]] bool ArePinTypesLinkableByDataType(PinTypeID aInputPinTypeID, PinTypeID aOutputPinTypeID);
		[[nodiscard]] Link ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2);

		[[nodiscard]] LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed = false);
		[[nodiscard]] std::vector<LinkID> GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed = false);
		[[nodiscard]] std::vector<LinkID> GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID);

	}



}