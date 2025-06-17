#include "FlyInternal.hpp"
#include "FlyClass.hpp"
#include "CustomEvent/FlyCustomEvent.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "DataType/FlyDataTypeManager.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "Execution/FlyNodeExecutor.hpp"
#include "FlyFlow.hpp"
#include "FlyWildcard.hpp"
#include "FlyFoundation.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"
#include "../Trait/FlyTraitObject.hpp"

namespace FLY_NAMESPACE
{

	namespace Internal
	{

		bool g_IsDebugging = false;
		MemoryArena<1024> g_FrameArena;
		MemoryArena<10000> g_EditArena;

		Foundation& GetFoundation()
		{
			return Foundation::GetInstance();
		}

		NodeExecutor& GetNodeExecutor()
		{
			return GetFoundation().GetNodeExecutor();
		}

		DataTypeManager& GetDataTypeManager()
		{
			return GetFoundation().GetDataTypeManager();
		}

		NodeTypeManager& GetNodeTypeManager()
		{
			return GetFoundation().GetNodeTypeManager();
		}

		PinTypeManager& GetPinTypeManager()
		{
			return GetFoundation().GetPinTypeManager();
		}

		TraitManager& GetTraitManager()
		{
			return GetFoundation().GetTraitManager();
		}

		bool& IsDebugging()
		{
			return g_IsDebugging;
		}

		MemoryArena<1024>& GetFrameMemoryArena()
		{
			return g_FrameArena;
		}

		MemoryArena<10000>& GetEditMemoryArena()
		{
			return g_EditArena;
		}

		MemoryPool& GetMemoryPool()
		{
			return GetFoundation().GetMemoryPool();
		}

		EventGraph& GetNodeGraphCopy()
		{
			return GetFoundation().GetNodeGraphCopy();
		}
	}

	struct PinRef
	{
		NodeGraph* mNodeGraph = nullptr;
		PinID mPinID;

		friend bool operator<(const PinRef& a, const PinRef& b)
		{
			if (a.mNodeGraph == b.mNodeGraph)
			{
				return a.mPinID < b.mPinID;
			}
			return a.mNodeGraph < b.mNodeGraph;
		}

		friend bool operator==(const PinRef& a, const PinRef& b)
		{
			if (a.mNodeGraph == b.mNodeGraph)
			{
				return a.mPinID == b.mPinID;
			}
			return a.mNodeGraph == b.mNodeGraph;
		}

		explicit operator bool() const
		{
			return mPinID != InvalidID<PinID>();
		}
	};


	struct PinRefHasher final
	{
		size_t operator()(const PinRef& aPinRef) const
		{
			return reinterpret_cast<size_t>(&aPinRef.mNodeGraph) + static_cast<size_t>(aPinRef.mPinID);
		}
	};

	PinRef gActivePinLastFrame;
	PinRef gActivePinThisFrame;
	Command gChangePinValueCommand;

	namespace Internal
	{

		NodeGraph& GetNodeGraph(const NodeGraphVariantHandle& aNodeGraphVariant)
		{
			return std::visit(Visitor{
				[](EventGraph* aEventGraph) -> NodeGraph&
				{
					return aEventGraph->GetNodeGraph();
				},
				[](FunctionIDWrapper aFunctionIDWrapper) -> NodeGraph&
				{
					return GetNodeTypeManager().GetFunction(aFunctionIDWrapper.mID).GetNodeGraph();
				}
				}, aNodeGraphVariant);
		}

		const Pin& GetPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.GetPin(aPinID);
		}

		Pin& GetPin(const PinID aPinID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.GetPin(aPinID);
		}

		const PinType& GetPinType(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			const Pin& pin = GetPin(aPinID, aNodeGraph);
			return GetPinTypeManager().GetPinType(pin.GetTypeID());
		}

		const PinType& GetPinType(const Pin& aPin)
		{
			return GetPinTypeManager().GetPinType(aPin.GetTypeID());
		}

		const PinType& GetPinType(const PinTypeID aPinTypeID)
		{
			return GetPinTypeManager().GetPinType(aPinTypeID);
		}

		Node& GetNode(const NodeID aNodeID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.GetNode(aNodeID);
		}

		const Node& GetNode(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.GetNode(aNodeID);
		}

		const NodeType& GetNodeType(const Node& aNode)
		{
			return GetNodeTypeManager().GetNodeType(aNode.GetTypeID());
		}

		const NodeType& GetNodeType(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			const Node& node = GetNode(aNodeID, aNodeGraph);
			return GetNodeTypeManager().GetNodeType(node.GetTypeID());
		}

		NodeType& GetNodeType(const NodeTypeID aNodeTypeID)
		{
			return GetNodeTypeManager().GetNodeType(aNodeTypeID);
		}

		const DataType* GetDataTypeByID(const DataTypeID aDataTypeID)
		{
			return GetDataTypeManager().Find(aDataTypeID);
		}

		const DataType* GetDataTypeByID(const GenericDataTypeID aDataTypeID)
		{
			return GetDataTypeManager().Find(aDataTypeID);
		}

		Class& GetClassByID(const ClassID aClassID)
		{
			return GetDataTypeManager().GetClass(aClassID);
		}

		Trait& GetTraitByID(const TraitID aTraitID)
		{
			return GetTraitManager().GetTrait(aTraitID);
		}

		static void InitializeSubPinsRecursively(const eIODirection aIODirection, const std::vector<PinTypeID>& aPinTypeIDs);

		static void InitializeSubPinsForPin(const PinTypeID aPinTypeID)
		{
			PinTypeManager& pinTypeManager = GetPinTypeManager();
			const DataTypeManager& dataTypeManager = GetDataTypeManager();
			const eIODirection ioDirection = pinTypeManager.GetPinType(aPinTypeID).GetIODirection();
			if (const DataType* dataType = dataTypeManager.Find(pinTypeManager.GetPinType(aPinTypeID).GetDataTypeID()))
			{
				for (const Variable& variable : dataType->GetVariableContainer())
				{
					if (const DataType* dataType2 = dataTypeManager.Find(variable.GetDataTypeID()))
					{
						const PinTypeID newSubPinTypeID = pinTypeManager.CreatePinType(variable.Name(), ioDirection, variable.GetDataTypeID(), dataTypeManager.GetSetPinValueFunction(variable.GetDataTypeID(), ioDirection), dataTypeManager.GetSetPinValueFromPinFunction(variable.GetDataTypeID(), ioDirection));

						pinTypeManager.GetPinType(aPinTypeID).AddSplitPinTypeID(newSubPinTypeID);
					}
				}
				InitializeSubPinsRecursively(ioDirection, pinTypeManager.GetPinType(aPinTypeID).GetSplitPinTypeIDs());
			}

		}

		static void InitializeSubPinsRecursively(const eIODirection aIODirection, const std::vector<PinTypeID>& aPinTypeIDs)
		{
			PinTypeManager& pinTypeManager = GetPinTypeManager();
			const DataTypeManager& dataTypeManager = GetDataTypeManager();

			for (const PinTypeID pinTypeID : aPinTypeIDs)
			{
				if (const DataType* dataType = dataTypeManager.Find(pinTypeManager.GetPinType(pinTypeID).GetDataTypeID()))
				{
					for (const Variable& variable : dataType->GetVariableContainer())
					{
						if (const DataType* dataType2 = dataTypeManager.Find(variable.GetDataTypeID()))
						{
							const PinTypeID newSubPinTypeID = pinTypeManager.CreatePinType(variable.Name(), aIODirection, variable.GetDataTypeID(), dataTypeManager.GetSetPinValueFunction(variable.GetDataTypeID(), aIODirection), dataTypeManager.GetSetPinValueFromPinFunction(variable.GetDataTypeID(), aIODirection));

							pinTypeManager.GetPinType(pinTypeID).AddSplitPinTypeID(newSubPinTypeID);
						}
					}
					InitializeSubPinsRecursively(aIODirection, pinTypeManager.GetPinType(pinTypeID).GetSplitPinTypeIDs());
				}

			}
		}

		void InitializeSubPins()
		{
			const std::vector<NodeType>& nodeTypes = GetNodeTypeManager().GetNodeTypes();

			for (const NodeType& nodeType : nodeTypes)
			{
				InitializeSubPinsRecursively(eIODirection::Input, nodeType.GetInputPinTypeIDs());
				InitializeSubPinsRecursively(eIODirection::Output, nodeType.GetOutputPinTypeIDs());
			}
		}

		PinTypeID CreatePinType(std::string_view aName, eIODirection aIODirection, GenericDataTypeID aDataTypeID)
		{
			const DataTypeManager& dataTypeManager = GetDataTypeManager();
			PinTypeManager& pinTypeManager = GetPinTypeManager();

			const PinTypeID newPinTypeID = pinTypeManager.CreatePinType(aName, aIODirection, aDataTypeID,
				dataTypeManager.GetSetPinValueFunction(aDataTypeID, aIODirection),
				dataTypeManager.GetSetPinValueFromPinFunction(aDataTypeID, aIODirection));

			InitializeSubPinsForPin(newPinTypeID);

			return newPinTypeID;
		}

		void BindNodeToEvent(const NodeID aNodeID, EventGraph& anEventGraph, CommandTracker* const aCommandTracker)
		{
			struct BindData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				EventGraph* mEventGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mEventGraph = &anEventGraph;

			auto doCommandFunction = [](const BindData& aData) -> void
				{
					aData.mEventGraph->BindNodeToEvent(aData.mNodeID);
				};

			auto undoCommandFunction = [](const BindData& aData) -> void
				{
					aData.mEventGraph->UnbindNodeFromEvent(aData.mNodeID);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(std::move(data), doCommandFunction, undoCommandFunction, "Bind Node To Event"));
			}
		}

		void BreakNode()
		{

		}

		DataTypeID CreateStruct(std::string_view aName)
		{
			const DataTypeID dataTypeID = GetDataTypeManager().CreateStruct(aName);
			DataType& dataType = *GetDataTypeManager().Find(dataTypeID);

			const NodeTypeID breakNodeTypeID = RegisterSystemNodeType(BreakNode, NodeCreationData{ .mName = "Default/Break " + dataType.Name() });
			AddPinTypeToNodeType(breakNodeTypeID, GenericDataTypeID{ dataTypeID }, eIODirection::Input, dataType.Name());
			dataType.SetBreakerNodeTypeID(breakNodeTypeID);

			return dataTypeID;
		}

		/*void SetStructName(const StructID aStructID, std::string_view aName, CommandTracker* const aCommandTracker)
		{
			struct SetStructNameData final
			{
				StructID mStructID;
				std::string mNewName;
				std::string mOldName;
			} data;

			data.mStructID = aStructID;
			data.mNewName = aName;
			data.mOldName = GetStructByID(aStructID).mName;

			auto doCommandFunction = [](const SetStructNameData& aData) -> void
				{
					GetStructByID(aData.mStructID).mName = aData.mNewName;
				};

			auto undoCommandFunction = [](const SetStructNameData& aData) -> void
				{
					GetStructByID(aData.mStructID).mName = aData.mOldName;
				};

			if (aCommandTracker)
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Set Class Name"));
			}
			else
			{
				doCommandFunction(data);
			}
		}*/

		ClassID CreateClass(const GenericDataTypeID aTargetID, const std::string_view aName)
		{
			const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&aTargetID.mID);
			return GetDataTypeManager().CreateClass(*dataTypeID, aName);
		}

		void SetClassName(const ClassID aClassID, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			struct SetClassNameData final
			{
				ClassID mClassID;
				std::string mNewName;
				std::string mOldName;
			} data;

			data.mClassID = aClassID;
			data.mNewName = aName;
			data.mOldName = GetClassByID(aClassID).mName;

			auto doCommandFunction = [](const SetClassNameData& aData) -> void
				{
					GetClassByID(aData.mClassID).mName = aData.mNewName;
				};

			auto undoCommandFunction = [](const SetClassNameData& aData) -> void
				{
					GetClassByID(aData.mClassID).mName = aData.mOldName;
				};

			if (aCommandTracker)
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Set Class Name"));
			}
			else
			{
				doCommandFunction(data);
			}
		}

		ClassInstance& CreateClassInstance(const ClassID aClassID)
		{
			Class& c = GetClassByID(aClassID);

			return *c.mClassInstances.emplace_back(HeapObject<ClassInstance>(aClassID));
		}

		void DestroyClassInstance(ClassInstance& aClassInstance)
		{
			Class& c = GetClassByID(aClassInstance.mClassID);

			std::erase_if(c.mClassInstances,
				[eraseClassInstance = aClassInstance](const auto& aClassInstance) { return &eraseClassInstance == aClassInstance.Get(); });
		}

		TraitID CreateTrait(std::string_view aName)
		{
			return GetTraitManager().CreateTrait(aName);
		}

		Flow InputTraitNode()
		{
			return Flow(true);
		}

		void OutputTraitNode(Flow)
		{

		}

		void CreateTraitImplementation(const DataTypeID aDataTypeID, const TraitID aTraitID)
		{
			TraitManager& traitManager = GetTraitManager();
			traitManager.CreateTraitImplementation(aDataTypeID, aTraitID);
			TraitImplementation* traitImplementaion = traitManager.GetTraitImplementation(aDataTypeID, aTraitID);
			const Trait& trait = traitManager.GetTrait(aTraitID);
			traitImplementaion;
			trait;

		}

		CustomEventID CreateCustomEvent(const std::string_view aName)
		{
			return GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(const std::string_view aName)
		{
			const FunctionID id = GetNodeTypeManager().CreateFunction(aName);

			Function& createdFunction = GetNodeTypeManager().GetFunction(id);

			[[maybe_unused]] const NodeID inputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.GetInputNodeTypeID());
			[[maybe_unused]] const NodeID outputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.GetOutputNodeTypeID());

			return id;
		}

		NodeID CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Position");
			}

			NodeGraph& nodeGraph = GetNodeGraph(aNodeGraphVariant);
			const NodeID nodeID = nodeGraph.GetCurrentNodeID();
			AddNode(nodeGraph, GetNodeTypeManager().CreateNode(nodeGraph, nodeID, aNodeTypeID), nodeID, aCommandTracker);

			SetNodePosition(nodeID, aPosition, nodeGraph, aCommandTracker);

			std::visit(
				[&](auto&& aType) -> void
				{
					using Type = std::decay_t<decltype(aType)>;
					if constexpr (std::same_as<Type, EventGraph*>)
					{
						BindNodeToEvent(nodeID, *aType, aCommandTracker);
					}

				}, aNodeGraphVariant
			);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}


		std::optional<NodeID> CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, const std::string_view aName, const Vec2 aPosition, const bool aCreateIfNameNotFound, CommandTracker* const aCommandTracker)
		{
			const NodeTypeID typeID = GetNodeTypeManager().GetTypeID(aName);
			if (typeID != NodeTypeID{ 0 } && !aCreateIfNameNotFound)
			{
				return std::nullopt;
			}
			return CreateNode(aNodeGraphVariant, typeID, aPosition, aCommandTracker);
		}

		NodeID CreateNodeAutoLink(const NodeGraphVariantHandle& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const PinID aConnectingPinID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Auto Link");
			}

			const NodeID createdNodeID = Internal::CreateNode(aNodeGraphVariant, aNodeTypeID, aPosition, aCommandTracker);

			const Pin& createdFromPin = GetNodeGraph(aNodeGraphVariant).GetPin(aConnectingPinID);
			const PinType& pinType = GetPinTypeManager().GetPinType(createdFromPin.GetTypeID());
			const Node& createdNode = GetNodeGraph(aNodeGraphVariant).GetNode(createdNodeID);

			const std::vector<PinID>& pinIDs = SelectByIODirection(pinType.GetIODirection(), createdNode.GetOutputPins(), createdNode.GetInputPins());

			for (const PinID pinID : pinIDs)
			{
				if (Internal::TryCreateLink(GetNodeGraph(aNodeGraphVariant), pinID, aConnectingPinID, aCommandTracker) != InvalidID<LinkID>())
				{
					break;
				}
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return createdNodeID;
		}

		NodeID CreateGetterNode(NodeGraph& aNodeGraph, const VarID aVarID, Class& aClass, const DataTypeID aDataTypeID, Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Getter Node");
			}

			const NodeID nodeID = aNodeGraph.GetCurrentNodeID();
			AddNode(aNodeGraph, GetNodeTypeManager().CreateGetterNode(aNodeGraph, nodeID, aDataTypeID), nodeID, aCommandTracker);

			Internal::SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			Internal::BindVariable(aClass, CreateContextualNodeRef(nodeID, aNodeGraph), aVarID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateSetterNode(NodeGraph& aNodeGraph, const VarID aVarID, Class& aClass, const DataTypeID aDataTypeID, Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Setter Node");
			}

			const NodeID nodeID = aNodeGraph.GetCurrentNodeID();
			AddNode(aNodeGraph, GetNodeTypeManager().CreateSetterNode(aNodeGraph, nodeID, aDataTypeID), nodeID, aCommandTracker);

			Internal::SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			Internal::BindVariable(aClass, CreateContextualNodeRef(nodeID, aNodeGraph), aVarID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateTraitNode(NodeGraph& aNodeGraph, const TraitID aTraitID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID nodeID = aNodeGraph.GetCurrentNodeID();
			AddNode(aNodeGraph, GetNodeTypeManager().CreateTraitNode(aNodeGraph, nodeID, aTraitID, aDataTypeID), nodeID, aCommandTracker);
			return nodeID;
		}

		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID nodeID = aNodeGraph.GetCurrentNodeID();
			AddNode(aNodeGraph, GetNodeTypeManager().CreateOperatorNode(aNodeGraph, nodeID, aOperatorTrait, aDataTypeID), nodeID, aCommandTracker);
			return nodeID;
		}

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* const aCommandTracker)
		{
			aNodeGraph.AddNode(aNode);

			Node& createdNode = aNodeGraph.GetNode(aNodeID);

			NodeType& nodeType = GetNodeTypeManager().GetNodeType(createdNode.GetTypeID());
			nodeType.GetNodeRefs().push_back(CreateContextualNodeRef(aNodeID, aNodeGraph));

			struct CreateNodeData
			{
				NodeID mNodeID;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const auto& aData) -> void
				{
					GetNode(aData.mNodeID, *aData.mNodeGraph).SetIsDestroyed(false);
				};

			auto undoCommandFunction = [](const auto& aData) -> void
				{
					GetNode(aData.mNodeID, *aData.mNodeGraph).SetIsDestroyed(true);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(std::move(data), doCommandFunction, undoCommandFunction, "Create Node"));
			}
		}

		void DestroyNode(NodeGraph& aNodeGraph, const NodeID aNodeID, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Node + Conncected Links");

			}

			struct DestroyNodeData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;


			auto doCommandFunction = [](const auto& aData) -> void
				{
					GetNode(aData.mNodeID, *aData.mNodeGraph).SetIsDestroyed(true);
				};

			auto undoCommandFunction = [](const auto& aData) -> void
				{
					GetNode(aData.mNodeID, *aData.mNodeGraph).SetIsDestroyed(false);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Destroy Node"));
			}

			for (const LinkID linkID : GetLinkIDsByNode(aNodeGraph, aNodeID))
			{
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyNodes(const std::span<GlobalNodeRef> aNodeRefs, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Nodes");
			}

			for (const GlobalNodeRef& nodeRef : aNodeRefs)
			{
				DestroyNode(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyNodes(const std::span<NodeRef> aNodeRefs, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Nodes");
			}

			for (const NodeRef& nodeRef : aNodeRefs)
			{
				DestroyNode(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyNodes(const std::span<NodeID> aNodeIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Nodes");
			}

			for (const NodeID nodeID : aNodeIDs)
			{
				DestroyNode(aNodeGraph, nodeID, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyLinks(const std::span<LinkID> aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Links");
			}

			for (const LinkID linkID : aLinkIDs)
			{
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroySelection(const std::span<NodeID> aNodeIDs, const std::span<LinkID> aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Selection");
			}

			DestroyLinks(aLinkIDs, aNodeGraph, aCommandTracker);
			DestroyNodes(aNodeIDs, aNodeGraph, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			const Vec2 oldPos = aNodeGraph.GetNode(aNodeID).GetPosition();
			SetNodePosition(aNodeID, aPosition, oldPos, aNodeGraph, aCommandTracker);
		}

		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{

			struct SetNodePositionData final
			{
				NodeID mNodeID;
				Vec2 mOldPos;
				Vec2 mNewPos;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			if (aPosition == aOldPosition)
			{
				return;
			}


			data.mNodeID = aNodeID;
			data.mOldPos = aOldPosition;
			data.mNewPos = aPosition;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const SetNodePositionData& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.GetPosition() = aData.mNewPos;
				};

			auto undoCommandFunction = [](const SetNodePositionData& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.GetPosition() = aData.mOldPos;
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Set Node Position"));
			}
		}

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			if (aNodeDragData.empty())
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Drag nodes");
			}

			for (const auto& [nodeID, dragData] : aNodeDragData)
			{
				Internal::SetNodePosition(nodeID, dragData.mEndPos, dragData.mStartPos, aNodeGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		static std::vector<PinID> CreatePins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const eIODirection aIODirection, size_t aStartIndex)
		{
			const NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, nodeType.GetInputPinTypeIDs(), nodeType.GetOutputPinTypeIDs());
			std::vector<PinID> pinIDs;
			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinIDs;
		}

		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
		{
			return CreatePins(aNodeGraph, aNodeID, aNodeTypeID, eIODirection::Input, aStartIndex);
		}

		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
		{
			return CreatePins(aNodeGraph, aNodeID, aNodeTypeID, eIODirection::Output, aStartIndex);
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
		{
			const auto dataTypeID = GetPinTypeManager().GetPinType(aPinTypeID).GetDataTypeID();

			void* const dataPtr = GetDataTypeManager().AllocateData(dataTypeID, aNodeGraph.GetMemoryArena());

			return CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
		{
			const PinType& pinType = GetPinType(aPinTypeID);

			const PinID createdPinID = aNodeGraph.AddPin(Pin(aPinTypeID, aDataPtr, aNodeID));

			if (pinType.GetSplitPinTypeIDs().empty())
			{
				return createdPinID;
			}

			const DataType* dataType = GetDataTypeManager().Find(pinType.GetDataTypeID());
			if (!dataType)
			{
				return createdPinID;
			}

			std::vector<PinID> subPinIDs;
			subPinIDs.reserve(pinType.GetSplitPinTypeIDs().size());

			assert(pinType.GetSplitPinTypeIDs().size() == dataType->GetVariableContainer().GetVariableCount());
			for (size_t i = 0; i < pinType.GetSplitPinTypeIDs().size(); i++)
			{
				void* const dataPtr = reinterpret_cast<char*>(aDataPtr) + dataType->GetVariableContainer().GetVariable(i).GetByteOffset();

				const PinID createdSubPinID = CreatePin(aNodeGraph, aNodeID, pinType.GetSplitPinTypeIDs()[i], dataPtr);
				subPinIDs.push_back(createdSubPinID);

				aNodeGraph.GetPin(createdSubPinID).SetParentPinID(createdPinID);
			}

			Pin& createdPin = aNodeGraph.GetPin(createdPinID);
			createdPin.SetSplitPinIDs(std::move(subPinIDs));

			return createdPinID;
		}

		void ViewAndEditPinGeneric(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);

			if (pin.IsSplit())
			{
				assert(false);
				return;
			}

			const PinType& pinType = GetPinType(pin);

			const DataTypeManager& dataTypeManager = GetDataTypeManager();

			const void* const copyDataPtr = [aCommandTracker, &dataTypeManager, &pinType, &pin]() -> const void*
				{
					return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.GetDataTypeID(), Internal::GetFrameMemoryArena(), pin.GetDataPtr()) : nullptr;
				}();

			const ViewAndEditResult viewAndEditResult = dataTypeManager.ViewAndEditData(pinType.GetDataTypeID(), pin.GetDataPtr());

			if (!viewAndEditResult.mIsItemActive || !aCommandTracker)
			{
				return;
			}

			const PinRef pinRef{ .mNodeGraph = &aNodeGraph, .mPinID = aPinID };

			gActivePinThisFrame = pinRef;

			if (gActivePinLastFrame == gActivePinThisFrame)
			{
				return;
			}

			void* const previousDataPtr = GetDataTypeManager().AllocateData(pinType.GetDataTypeID(), Internal::GetEditMemoryArena(), copyDataPtr);

			struct EditPinData
			{
				PinID mPinID;
				void* mPreviousDataPtr = nullptr;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mPinID = aPinID;
			data.mPreviousDataPtr = previousDataPtr;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const EditPinData& aData) -> void
				{
					Pin& pin = GetPin(aData.mPinID, *aData.mNodeGraph);
					const PinType& pinType = GetPinType(pin);

					GetDataTypeManager().SwapData(pinType.GetDataTypeID(), pin.GetDataPtr(), aData.mPreviousDataPtr);
				};

			auto undoCommandFunction = [](const EditPinData& aData) -> void
				{
					Pin& pin = GetPin(aData.mPinID, *aData.mNodeGraph);
					const PinType& pinType = GetPinType(pin);

					GetDataTypeManager().SwapData(pinType.GetDataTypeID(), pin.GetDataPtr(), aData.mPreviousDataPtr);
				};

			gChangePinValueCommand = Command(data, doCommandFunction, undoCommandFunction, "Edit Pin");
		}


		/*void ViewAndEditPin(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);

			if (pin.mIsSplit)
			{
				assert(false);
				return;
			}

			const PinType& pinType = GetPinType(pin);

			const DataTypeManager& dataTypeManager = GetDataTypeManager();

			const void* const copyDataPtr = [aCommandTracker, &dataTypeManager, &pinType, &pin]() -> const void*
				{
					return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.mDataTypeID, Internal::GetFrameMemoryArena(), pin.mDataPtr) : nullptr;
				}();

			const ViewAndEditResult viewAndEditResult = dataTypeManager.ViewAndEditData(pinType.mDataTypeID, pin.mDataPtr);

			if (!viewAndEditResult.mIsItemActive || !aCommandTracker)
			{
				return;
			}

			const PinRef pinRef{ .mNodeGraph = &aNodeGraph, .mPinID = aPinID };

			gActivePinThisFrame = pinRef;

			if (gActivePinLastFrame == gActivePinThisFrame)
			{
				return;
			}

			void* const previousDataPtr = GetDataTypeManager().AllocateData(pinType.mDataTypeID, Internal::GetEditMemoryArena(), copyDataPtr);

			struct EditPinData
			{
				PinID mPinID;
				void* mPreviousDataPtr = nullptr;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mPinID = aPinID;
			data.mPreviousDataPtr = previousDataPtr;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const EditPinData& aData) -> void
				{
					Pin& pin = GetPin(aData.mPinID, *aData.mNodeGraph);
					const PinType& pinType = GetPinType(pin);

					GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, aData.mPreviousDataPtr);
				};

			auto undoCommandFunction = [](const EditPinData& aData) -> void
				{
					Pin& pin = GetPin(aData.mPinID, *aData.mNodeGraph);
					const PinType& pinType = GetPinType(pin);

					GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, aData.mPreviousDataPtr);
				};

			gChangePinValueCommand = Command(data, doCommandFunction, undoCommandFunction, "Edit Pin");
		}*/

		void ViewPinGeneric(PinID aPinID, const NodeGraph& aNodeGraph)
		{
			GetDataTypeManager().ViewData(GetPinType(aPinID, aNodeGraph).GetDataTypeID(), aNodeGraph.GetPin(aPinID).GetDataPtr());
		}

		/*void ViewPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			GetDataTypeManager().ViewData(GetPinType(aPinID, aNodeGraph).mDataTypeID, GetPin(aPinID, aNodeGraph).mDataPtr);
		}*/

		static void SplitPinInternal(const PinID aPinID, NodeGraph& aNodeGraph, const size_t aIndex)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);
			pin.SetIsSplit(true);
			Node& node = GetNode(pin.GetNodeID(), aNodeGraph);
			const PinType& pinType = GetPinType(aPinID, aNodeGraph);
			auto& nodePins = SelectByIODirection(pinType.GetIODirection(), node.GetSplitInputPins(), node.GetSplitOutputPins());

			nodePins.erase(begin(nodePins) + aIndex);
			nodePins.insert(begin(nodePins) + aIndex, begin(pin.GetSplitPinIDs()), end(pin.GetSplitPinIDs()));
		}

		static void RecombinePinInternal(const PinID aPinID, NodeGraph& aNodeGraph, const size_t aIndex)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);
			pin.SetIsSplit(false);
			Node& node = GetNode(pin.GetNodeID(), aNodeGraph);
			const PinType& pinType = GetPinType(aPinID, aNodeGraph);
			auto& nodePins = SelectByIODirection(pinType.GetIODirection(), node.GetSplitInputPins(), node.GetSplitOutputPins());

			nodePins.erase(nodePins.begin() + aIndex, begin(nodePins) + aIndex + pin.GetSplitPinIDs().size());
			nodePins.insert(nodePins.begin() + aIndex, aPinID);
		}

		void SplitPin(const PinID aPinID, NodeGraph& aNodeGraph, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);

			if (!pin.GetConnectedPinIDs().empty())
			{
				assert(false);
				return;
			}

			Node& node = GetNode(pin.GetNodeID(), aNodeGraph);

			const PinType& pinType = GetPinType(aPinID, aNodeGraph);

			auto& nodePins = SelectByIODirection(pinType.GetIODirection(), node.GetSplitInputPins(), node.GetSplitOutputPins());

			auto it = std::find(begin(nodePins), end(nodePins), aPinID);
			if (it == nodePins.end())
			{
				assert(false);
				return;
			}

			struct SplitPinData final
			{
				size_t mIndex = std::numeric_limits<size_t>::max();
				PinID mPinID = InvalidID<PinID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mIndex = std::distance(begin(nodePins), it);
			data.mPinID = aPinID;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const SplitPinData& aData) -> void
				{
					SplitPinInternal(aData.mPinID, *aData.mNodeGraph, aData.mIndex);
				};

			auto undoCommandFunction = [](const SplitPinData& aData) -> void
				{
					RecombinePinInternal(aData.mPinID, *aData.mNodeGraph, aData.mIndex);
				};

			if (aCommandTracker)
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Split Pin"));
			}
			else
			{
				doCommandFunction(data);
			}
		}

		void RecombinePin(const PinID aPinID, NodeGraph& aNodeGraph, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);

			if (!pin.GetConnectedPinIDs().empty())
			{
				assert(false);
				return;
			}

			Node& node = GetNode(pin.GetNodeID(), aNodeGraph);

			const PinType& pinType = GetPinType(aPinID, aNodeGraph);

			const auto& nodePins = SelectByIODirection(pinType.GetIODirection(), node.GetSplitInputPins(), node.GetSplitOutputPins());

			auto it = std::find(nodePins.begin(), nodePins.end(), pin.GetSplitPinIDs().front());
			if (it == nodePins.end())
			{
				assert(false);
				return;
			}

			struct RecombinePinData final
			{
				size_t mIndex = std::numeric_limits<size_t>::max();
				PinID mPinID;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mIndex = std::distance(nodePins.begin(), it);
			data.mPinID = aPinID;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const RecombinePinData& aData) -> void
				{
					RecombinePinInternal(aData.mPinID, *aData.mNodeGraph, aData.mIndex);
				};

			auto undoCommandFunction = [](const RecombinePinData& aData) -> void
				{
					SplitPinInternal(aData.mPinID, *aData.mNodeGraph, aData.mIndex);
				};

			if (aCommandTracker)
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Recombine Pin"));
			}
			else
			{
				doCommandFunction(data);
			}
		}

		void BeginFrame(CommandTracker* const aCommandTracker)
		{
			if (!aCommandTracker)
			{
				return;
			}

			if (gActivePinLastFrame && gActivePinLastFrame != gActivePinThisFrame)
			{
				aCommandTracker->RegisterCommand(Command(gChangePinValueCommand));
			}

			gActivePinLastFrame = gActivePinThisFrame;
			gActivePinThisFrame = PinRef{};


			Internal::GetFrameMemoryArena().Clear();
			GetNodeExecutor().GetDebugger().ClearTraversedLinks();
		}

		bool IsNodeReplacable(NodeGraph& aNodeGraph, NodeID aNodeID)
		{
			const NodeType& nodeType = GetNodeType(aNodeID, aNodeGraph);
			return nodeType.GetTraitID() != InvalidID<TraitID>();
			//return nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None;
		}

		void ActivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.GetLink(aLinkID);

			{
				Pin& inputPin = aNodeGraph.GetPin(link.GetInputPinID());
				auto it = std::find(begin(inputPin.GetConnectedPinIDs()), end(inputPin.GetConnectedPinIDs()), link.GetOutputPinID());
				if (it == end(inputPin.GetConnectedPinIDs()))
				{
					inputPin.AddConnectedPin(link.GetOutputPinID());
				}
			}
			{
				Pin& outputPin = aNodeGraph.GetPin(link.GetOutputPinID());
				auto it = std::find(begin(outputPin.GetConnectedPinIDs()), end(outputPin.GetConnectedPinIDs()), link.GetInputPinID());
				if (it == end(outputPin.GetConnectedPinIDs()))
				{
					outputPin.AddConnectedPin(link.GetInputPinID());
				}
			}

			link.SetIsDestroyed(false);
		}

		void DeactivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.GetLink(aLinkID);

			Pin& inputPin = aNodeGraph.GetPin(link.GetInputPinID());
			Pin& outputPin = aNodeGraph.GetPin(link.GetOutputPinID());

			inputPin.RemoveConnectedPin(link.GetOutputPinID());
			outputPin.RemoveConnectedPin(link.GetInputPinID());

			link.SetIsDestroyed(true);
		}

		LinkID TryCreateLink(NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, CommandTracker* const aCommandTracker)
		{
			const Link createdLink = ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded operator node
				const Pin& pin1 = aNodeGraph.GetPin(aPinID1);
				const Pin& pin2 = aNodeGraph.GetPin(aPinID2);

				const PinType& pinType1 = GetPinTypeManager().GetPinType(pin1.GetTypeID());
				const PinType& pinType2 = GetPinTypeManager().GetPinType(pin2.GetTypeID());

				if (pinType1.GetDataTypeID() == GenericDataTypeID{ GetDataTypeID<Wildcard>() })
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID1, aPinID2, aCommandTracker);
				}
				else if (pinType2.GetDataTypeID() == GenericDataTypeID{ GetDataTypeID<Wildcard>() })
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
				}

				return InvalidID<LinkID>();
			}

			return CreateLink(aNodeGraph, createdLink.GetInputPinID(), createdLink.GetOutputPinID(), aCommandTracker);
		}

		LinkID CreateLinkGeneric(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID, CommandTracker* const aCommandTracker)
		{
			assert(aInputPinID != InvalidID<PinID>());
			assert(aOutputPinID != InvalidID<PinID>());

			struct CreateLinkData final
			{
				LinkID mCreatedLinkID;
				LinkID mPreviousLinkID;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			const Pin& inputPin = aNodeGraph.GetPin(aInputPinID);
			const Pin& outputPin = aNodeGraph.GetPin(aOutputPinID);
			const PinType& inputPinType = GetPinTypeManager().GetPinType(inputPin.GetTypeID());
			[[maybe_unused]] const PinType& outputPinType = GetPinTypeManager().GetPinType(outputPin.GetTypeID());
			assert(inputPinType.GetIODirection() == eIODirection::Input);
			assert(outputPinType.GetIODirection() == eIODirection::Output);
			assert(GetDataTypeManager().AreDataTypesRelated(inputPinType.GetDataTypeID(), outputPinType.GetDataTypeID()));

			if (inputPinType.GetDataTypeID() != GenericDataTypeID{ GetDataTypeID<Flow>() })
			{
				const std::vector<LinkID> inputLinkIDs = GetLinkIDsByPin(aNodeGraph, aInputPinID);
				if (!inputLinkIDs.empty())
				{
					assert(inputLinkIDs.size() == 1);
					data.mPreviousLinkID = inputLinkIDs.front();
				}
			}
			else
			{
				const std::vector<LinkID> outputLinkIDs = GetLinkIDsByPin(aNodeGraph, aOutputPinID);
				if (!outputLinkIDs.empty())
				{
					assert(outputLinkIDs.size() == 1);
					data.mPreviousLinkID = outputLinkIDs.front();
				}
			}

			data.mNodeGraph = &aNodeGraph;
			data.mCreatedLinkID = aNodeGraph.AddLink(Link{ aInputPinID, aOutputPinID });

			auto doCommandFunction = [](const CreateLinkData& aData) -> void
				{
					if (aData.mPreviousLinkID != InvalidID<LinkID>())
					{
						DeactivateLink(*aData.mNodeGraph, aData.mPreviousLinkID);
					}

					ActivateLink(*aData.mNodeGraph, aData.mCreatedLinkID);
				};

			auto undoCommandFunction = [](const CreateLinkData& aData) -> void
				{
					DeactivateLink(*aData.mNodeGraph, aData.mCreatedLinkID);
					if (aData.mPreviousLinkID != InvalidID<LinkID>())
					{
						ActivateLink(*aData.mNodeGraph, aData.mPreviousLinkID);
					}
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Create Link"));
			}

			return data.mCreatedLinkID;
		}

		LinkID CreateLink(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID, CommandTracker* const aCommandTracker)
		{
			assert(aInputPinID != InvalidID<PinID>());
			assert(aOutputPinID != InvalidID<PinID>());

			struct CreateLinkData final
			{
				LinkID mCreatedLinkID;
				LinkID mPreviousLinkID;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			const Pin& inputPin = aNodeGraph.GetPin(aInputPinID);
			const Pin& outputPin = aNodeGraph.GetPin(aOutputPinID);
			const PinType& inputPinType = GetPinTypeManager().GetPinType(inputPin.GetTypeID());
			[[maybe_unused]] const PinType& outputPinType = GetPinTypeManager().GetPinType(outputPin.GetTypeID());
			assert(inputPinType.GetIODirection() == eIODirection::Input);
			assert(outputPinType.GetIODirection() == eIODirection::Output);
			assert(GetDataTypeManager().AreDataTypesRelated(inputPinType.GetDataTypeID(), outputPinType.GetDataTypeID()));

			if (inputPinType.GetDataTypeID() != GenericDataTypeID{ GetDataTypeID<Flow>() })
			{
				const std::vector<LinkID> inputLinkIDs = GetLinkIDsByPin(aNodeGraph, aInputPinID);
				if (!inputLinkIDs.empty())
				{
					assert(inputLinkIDs.size() == 1);
					data.mPreviousLinkID = inputLinkIDs.front();
				}
			}
			else
			{
				const std::vector<LinkID> outputLinkIDs = GetLinkIDsByPin(aNodeGraph, aOutputPinID);
				if (!outputLinkIDs.empty())
				{
					assert(outputLinkIDs.size() == 1);
					data.mPreviousLinkID = outputLinkIDs.front();
				}
			}

			data.mNodeGraph = &aNodeGraph;
			data.mCreatedLinkID = aNodeGraph.AddLink(Link{ aInputPinID, aOutputPinID });


			auto doCommandFunction = [](const CreateLinkData& aData) -> void
				{
					if (aData.mPreviousLinkID != InvalidID<LinkID>())
					{
						DeactivateLink(*aData.mNodeGraph, aData.mPreviousLinkID);
					}

					ActivateLink(*aData.mNodeGraph, aData.mCreatedLinkID);
				};

			auto undoCommandFunction = [](const CreateLinkData& aData) -> void
				{
					DeactivateLink(*aData.mNodeGraph, aData.mCreatedLinkID);
					if (aData.mPreviousLinkID != InvalidID<LinkID>())
					{
						ActivateLink(*aData.mNodeGraph, aData.mPreviousLinkID);
					}
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Create Link"));
			}

			return data.mCreatedLinkID;
		}

		void DestroyLink(NodeGraph& aNodeGraph, const LinkID aLinkID, CommandTracker* const aCommandTracker)
		{
			assert(aLinkID != InvalidID<LinkID>());

			struct DestroyLinkData final
			{
				LinkID mDestroyedLinkID;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mDestroyedLinkID = aLinkID;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const DestroyLinkData& aData) -> void
				{
					DeactivateLink(*aData.mNodeGraph, aData.mDestroyedLinkID);
				};

			auto undoCommandFunction = [](const DestroyLinkData& aData) -> void
				{
					ActivateLink(*aData.mNodeGraph, aData.mDestroyedLinkID);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Destroy Link"));
			}
		}

		void DestroyLinksByPin(NodeGraph& aNodeGraph, const PinID aPinID, CommandTracker* const aCommandTracker)
		{
			const Pin& pin = aNodeGraph.GetPin(aPinID);

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Links By Pin");
			}


			for (const PinID connectedPinID : pin.GetConnectedPinIDs())
			{
				const LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID);
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void UpdateStructBreakNode(GenericDataTypeID aDataTypeID)
		{
			const DataType* dataType = GetDataTypeByID(aDataTypeID);
			if (!dataType)
			{
				assert(false);
				return;
			}
			NodeType& nodeType = GetNodeType(dataType->GetBreakerNodeTypeID());
			nodeType.GetOutputPinTypeIDs().clear();

			for (const Variable& variable : dataType->GetVariableContainer())
			{
				AddPinTypeToNodeType(dataType->GetBreakerNodeTypeID(), aDataTypeID, eIODirection::Output, variable.Name());
			}
		}

		VarID CreateVariable(const GenericDataTypeID aParentDataTypeID, const GenericDataTypeID aDataTypeID, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			const DataTypeID* parentDataTypeID = std::get_if<DataTypeID>(&aParentDataTypeID.mID);
			if (!parentDataTypeID)
			{
				assert(false);
				return VarID{};
			}
			VariableContainer& variableContainer = GetDataTypeManager().Find(*parentDataTypeID)->GetVariableContainer();
			const VarID varID{ variableContainer.GetVariableCount() };

			variableContainer.AddVariable(Variable{});

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Variable");
			}

			SetVariableName(varID, variableContainer, aName, aCommandTracker);
			SetVariableDataType(varID, variableContainer, aDataTypeID, aCommandTracker);
			UpdateStructBreakNode(aParentDataTypeID);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			/*for (auto& classInstance : aClass.mClassInstances)
			{
				classInstance->mVariableContainerInstance.Mirror();
			}*/



			return varID;
		}

		VarID CreateVariable(VariableContainer& aVariableContainer, const GenericDataTypeID aDataTypeID, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			const VarID varID{ aVariableContainer.GetVariableCount() };

			aVariableContainer.AddVariable(Variable{});

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Variable");
			}

			SetVariableName(varID, aVariableContainer, aName, aCommandTracker);
			SetVariableDataType(varID, aVariableContainer, aDataTypeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			/*for (auto& classInstance : aClass.mClassInstances)
			{
				classInstance->mVariableContainerInstance.Mirror();
			}*/



			return varID;
		}

		void SetVariableDataType(const VarID aVarID, VariableContainer& aVariableContainer, const GenericDataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			Variable& variable = aVariableContainer.GetVariable(aVarID);

			void* const defaultValueDataPtr = GetDataTypeManager().AllocateData(aDataTypeID, aVariableContainer.GetMemoryArena());

			variable.SetDataTypeID(aDataTypeID);
			variable.SetDefaultValueDataPtr(defaultValueDataPtr);

			aCommandTracker;

			//DestroyVariableNodes(aVarID, aClass, aCommandTracker);
		}

		void SetVariableName(const VarID aVarID, VariableContainer& aVariableContainer, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			struct SetVariableNameData
			{
				std::string mNewName;
				std::string mOldName;
				VarID mVarID = InvalidID<VarID>();
				VariableContainer* mVariableContainer = nullptr;
			} data;

			data.mOldName = aVariableContainer.GetVariable(aVarID).Name();
			data.mNewName = aName;
			data.mVarID = aVarID;
			data.mVariableContainer = &aVariableContainer;

			auto doCommandFunction = [](const SetVariableNameData& aData) -> void
				{
					aData.mVariableContainer->GetVariable(aData.mVarID).Name() = aData.mNewName;
				};

			auto undoCommandFunction = [](const SetVariableNameData& aData) -> void
				{
					aData.mVariableContainer->GetVariable(aData.mVarID).Name() = aData.mOldName;
				};


			if (aCommandTracker)
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Set Variable Name"));
			}
			else
			{
				doCommandFunction(data);
			}
		}

		void DestroyVariable(const VarID aVarID, VariableContainer& aVariableContainer, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
			}

			struct DestroyVariableData
			{
				VarID mVarID = InvalidID<VarID>();
				VariableContainer* mVariableContainer = nullptr;
			} data;

			data.mVarID = aVarID;
			data.mVariableContainer = &aVariableContainer;

			auto doCommandFunction = [](const DestroyVariableData& aData) -> void
				{
					aData.mVariableContainer->GetVariable(aData.mVarID).SetIsDestroyed(true);
				};


			auto undoCommandFunction = [](const DestroyVariableData& aData) -> void
				{
					aData.mVariableContainer->GetVariable(aData.mVarID).SetIsDestroyed(false);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Destroy Variable"));
			}

			//DestroyNodes(GetNodeRefsByVariableRef(VariableRef(aVarID, aClass)), aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		/*void DestroyVariableNodes(const VarID aVarID, VariableContainer& aVariableContainer, CommandTracker* const aCommandTracker)
		{
			DestroyNodes(GetNodeRefsByVariableRef(VariableRef(aVarID, aClass)), aCommandTracker);
		}*/

		void ViewAndEditVariableDefaultValue(const VarID aVarID, VariableContainer& aVariableContainer, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			aVarID; aVariableContainer;
			Variable& variable = aVariableContainer.GetVariable(aVarID);

			ViewAndEditResult r = GetDataTypeManager().ViewAndEditData(variable.GetDataTypeID(), variable.GetDefaultValueDataPtr());
			r;
		}

		void ViewAndEditClassInstanceVariableDefaultValue(ClassInstance& aClassInstance, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			aClassInstance;
			/*const std::vector<Variable>& variables = aClassInstance.mVariableContainerInstance.mVariableContainer->mVariables;
			std::vector<VariableInstance>& variableInstances = aClassInstance.mVariableContainerInstance.mVariableInstances;

			assert(variables.size() == variableInstances.size());

			for (size_t i = 0; i < variableInstances.size(); ++i)
			{
				const Variable& variable = variables[i];
				VariableInstance& variableInstance = variableInstances[i];
				GetDataTypeManager().ViewAndEditData(variable.mDataTypeID, variableInstance.mDefaultValueDataPtr);
			}*/
		}

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, const VarID aVarID, CommandTracker* const aCommandTracker)
		{
			const GlobalNodeRef gNodeRef = CreateGlobalNodeRef(aNodeRef, aClass);
			struct BindVarData
			{
				GlobalNodeRef mNodeRef;
				VariableRef mVarRef;
			} data;

			data.mNodeRef = gNodeRef;
			data.mVarRef = VariableRef(aVarID, aClass);

			auto doCommandFunction = [](const BindVarData& aData) -> void
				{
					GetFoundation().mNodeRefToVarRef[aData.mNodeRef] = aData.mVarRef;
				};

			auto undoCommandFunction = [](const BindVarData& aData) -> void
				{
					GetFoundation().mNodeRefToVarRef.erase(aData.mNodeRef);
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Bind Node To Variable"));
			}
		}

		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker)
		{
			const GlobalNodeRef gNodeRef = CreateGlobalNodeRef(aNodeRef, aClass);
			if (!GetFoundation().mNodeRefToVarRef.contains(gNodeRef))
			{
				return;
			}

			struct UnbindVarData
			{
				GlobalNodeRef mNodeRef;
				VariableRef mVarRef;
			} data;

			data.mNodeRef = gNodeRef;
			data.mVarRef = GetVariableRefByNodeRef(gNodeRef);

			auto doCommandFunction = [](const UnbindVarData& aData) -> void
				{
					GetFoundation().mNodeRefToVarRef.erase(aData.mNodeRef);
				};

			auto undoCommandFunction = [](const UnbindVarData& aData) -> void
				{
					GetFoundation().mNodeRefToVarRef[aData.mNodeRef] = aData.mVarRef;
				};

			if (!aCommandTracker)
			{
				doCommandFunction(data);
			}
			else
			{
				aCommandTracker->DoCommand(Command(data, doCommandFunction, undoCommandFunction, "Unbind Variable"));
			}
		}

		void SetPinTypeName(const PinTypeID aPinTypeID, const std::string_view aName)
		{
			PinType& pinType = GetPinTypeManager().GetPinType(aPinTypeID);

			pinType.SetName(std::string(aName));
		}

		PinTypeID AddPinTypeToNodeType(const NodeTypeID aNodeTypeID, const GenericDataTypeID aDataTypeID, const eIODirection aIODirection, std::string_view aPinName)
		{
			NodeType& nodeType = GetNodeType(aNodeTypeID);

			const PinTypeID createdPinTypeID = CreatePinType(aPinName, aIODirection, aDataTypeID);

			std::vector<PinTypeID>& pinTypeIDs = aIODirection == eIODirection::Input ? nodeType.GetInputPinTypeIDs() : nodeType.GetOutputPinTypeIDs();
			pinTypeIDs.push_back(createdPinTypeID);

			for (const NodeRef& nodeRef : nodeType.GetNodeRefs())
			{
				Node& node = GetNode(nodeRef.GetNodeID(), nodeRef.GetNodeGraph());

				std::vector<PinID>& pinIDs = SelectByIODirection(aIODirection, node.GetInputPins(), node.GetOutputPins());
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), createdPinTypeID);

				pinIDs.push_back(createdPinID);
			}



			return createdPinTypeID;
		}


		static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const GenericDataTypeID aDataTypeID, const eIODirection aIODirection)
		{
			NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			PinTypeManager& pinTypeManager = GetPinTypeManager();

			NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
			std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, nodeType.GetInputPinTypeIDs(), nodeType.GetOutputPinTypeIDs());
			const PinTypeID oldPinTypeID = pinTypeIDs[aIndex];
			const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

			const PinTypeID newPinTypeID = CreatePinType(oldPinType.GetName(), aIODirection, aDataTypeID);

			pinTypeIDs[aIndex] = newPinTypeID;

			const std::vector<NodeRef>& nodeRefs = nodeType.GetNodeRefs();
			for (const NodeRef& nodeRef : nodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().GetNode(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByIODirection(aIODirection, node.GetInputPins(), node.GetOutputPins());
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), newPinTypeID);
				pinIDs[aIndex] = createdPinID;
			}
		}

		static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const eIODirection aIODirection)
		{
			NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);

			std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, nodeType.GetInputPinTypeIDs(), nodeType.GetOutputPinTypeIDs());

			assert(aIndex < pinTypeIDs.size());

			pinTypeIDs.erase(pinTypeIDs.begin() + aIndex);

			for (const NodeRef& nodeRef : nodeType.GetNodeRefs())
			{
				Node& node = nodeRef.GetNodeGraph().GetNode(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByIODirection(aIODirection, node.GetInputPins(), node.GetOutputPins());
				DestroyLinksByPin(nodeRef.GetNodeGraph(), pinIDs.at(aIndex), nullptr);
				pinIDs.erase(pinIDs.begin() + aIndex);
			}
		}

		void SetCustomEventName(const CustomEventID aCustomEventID, std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);
			NodeType& executorNodeType = GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
			NodeType& callerNodeType = GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

			const std::string nameDirectory = GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
			executorNodeType.SetName(nameDirectory + std::string(aName));
			callerNodeType.SetName(nameDirectory + "Call " + std::string(aName));
		}

		void AddPinTypeToCustomEvent(const CustomEventID aCustomEventID, const GenericDataTypeID aDataTypeID, const std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			AddPinTypeToNodeType(customEvent.GetExecutorTypeID(), aDataTypeID, eIODirection::Output, aPinName);
			AddPinTypeToNodeType(customEvent.GetCallerTypeID(), aDataTypeID, eIODirection::Input, aPinName);
		}

		void SetPinDataTypeAtIndexCustomEvent(const CustomEventID aCustomEventID, const GenericDataTypeID aDataTypeID, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, aDataTypeID, eIODirection::Output);
			SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, aDataTypeID, eIODirection::Input);
		}

		void SetPinNameAtIndexCustomEvent(const CustomEventID aCustomEventID, const std::string_view aName, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			const CustomEvent& customEvent = nodeTypeManager.GetCustomEvent(aCustomEventID);

			{
				const NodeType& callerNodeType = nodeTypeManager.GetNodeType(customEvent.GetCallerTypeID());

				SetPinTypeName(callerNodeType.GetInputPinTypeIDs()[aIndex], aName);
			}

			{
				const NodeType& executorNodeType = nodeTypeManager.GetNodeType(customEvent.GetExecutorTypeID());

				SetPinTypeName(executorNodeType.GetOutputPinTypeIDs()[aIndex], aName);
			}
		}

		void DeletePinAtIndexCustomEvent(const CustomEventID aCustomEventID, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}

			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, eIODirection::Input);
			DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, eIODirection::Output);
		}

		void AddPinTypeToFunction(const FunctionID aFunctionID, const GenericDataTypeID aDataTypeID, const eIODirection aIODirection, std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);


			AddPinTypeToNodeType(function.GetCallerNodeTypeID(), aDataTypeID, aIODirection, aPinName);

			const NodeTypeID inputOutputNodeTypeID = SelectByIODirection(aIODirection, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
			AddPinTypeToNodeType(inputOutputNodeTypeID, aDataTypeID, InvertIODirection(aIODirection), aPinName);
		}

		void SetPinDataTypeAtIndexFunction(const FunctionID aFunctionID, const GenericDataTypeID aDataTypeID, const size_t aIndex, const eIODirection aIODirection, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = GetNodeTypeManager().GetFunction(aFunctionID);

			SetPinAtIndexNodeType(function.GetCallerNodeTypeID(), aIndex, aDataTypeID, aIODirection);

			const NodeTypeID inputOutputNodeTypeID = SelectByIODirection(aIODirection, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
			SetPinAtIndexNodeType(inputOutputNodeTypeID, aIndex, aDataTypeID, InvertIODirection(aIODirection));
		}

		void SetPinNameAtIndexFunction(const FunctionID aFunctionID, const std::string_view aName, const size_t aIndex, const eIODirection aIODirection, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);

			{
				const NodeType& callerNodeType = nodeTypeManager.GetNodeType(function.GetCallerNodeTypeID());

				const std::vector<PinTypeID>& callerPinTypeIDs = SelectByIODirection(aIODirection, callerNodeType.GetInputPinTypeIDs(), callerNodeType.GetOutputPinTypeIDs());
				SetPinTypeName(callerPinTypeIDs.at(aIndex), aName);
			}

			{
				const NodeType& inputNodeType = nodeTypeManager.GetNodeType(function.GetInputNodeTypeID());
				const NodeType& outputNodeType = nodeTypeManager.GetNodeType(function.GetOutputNodeTypeID());

				const std::vector<PinTypeID>& inputOutputPinTypeIDs = SelectByIODirection(aIODirection, inputNodeType.GetOutputPinTypeIDs(), outputNodeType.GetInputPinTypeIDs());
				SetPinTypeName(inputOutputPinTypeIDs.at(aIndex), aName);
			}
		}

		void DeletePinAtIndexFunction(const FunctionID aFunctionID, const size_t aIndex, const eIODirection aIODirection, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = GetNodeTypeManager().GetFunction(aFunctionID);

			DeletePinAtIndexNodeType(function.GetCallerNodeTypeID(), aIndex, aIODirection);

			const NodeTypeID inputOutputNodeTypeID = SelectByIODirection(aIODirection, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
			DeletePinAtIndexNodeType(inputOutputNodeTypeID, aIndex, InvertIODirection(aIODirection));
		}

		void ReplaceNode(NodeGraph& aNodeGraph, const NodeID aReplacedNodeID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeType& replacedNodeType = GetNodeType(aReplacedNodeID, aNodeGraph);

			if (replacedNodeType.GetTraitID() == InvalidID<TraitID>())
			{
				return;
			}

			//const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&aDataTypeID.mID);

			/*if (!dataTypeID)
			{
				return;
			}*/

			const bool canReplace = GetNodeTypeManager().CanCreateNodeByTrait(replacedNodeType.GetTraitID(), aDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateTraitNode(aNodeGraph, replacedNodeType.GetTraitID(), aDataTypeID, aCommandTracker);
			const Node& replacedNode = GetNode(aReplacedNodeID, aNodeGraph);
			SetNodePosition(createdNodeID, replacedNode.GetPosition(), aNodeGraph, aCommandTracker);
			DestroyNode(aNodeGraph, aReplacedNodeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, const PinID aWildcardPinID, const PinID aConnectedPinID, CommandTracker* const aCommandTracker)
		{
			const Pin& wildcardPin = GetPin(aWildcardPinID, aNodeGraph);
			const Pin& connectedPin = GetPin(aConnectedPinID, aNodeGraph);

			const NodeID wildcardNodeID = wildcardPin.GetNodeID();

			const NodeType& wildcardNodeType = GetNodeTypeManager().GetNodeType(aNodeGraph.GetNode(wildcardNodeID).GetTypeID());

			if (wildcardNodeType.GetOperatorTrait() == eNodeOperatorTrait::None)
			{
				return;
			}


			const PinType& wildcardPinType = GetPinType(wildcardPin.GetTypeID());
			const PinType& connectedPinType = GetPinType(connectedPin.GetTypeID());
			const GenericDataTypeID dtID = connectedPinType.GetDataTypeID();
			const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&dtID.mID);

			if (!dataTypeID)
			{
				return;
			}

			const bool canReplace = GetNodeTypeManager().CanCreateOperatorNode(wildcardNodeType.GetOperatorTrait(), *dataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, wildcardNodeType.GetOperatorTrait(), *dataTypeID, aCommandTracker);

			DestroyNode(aNodeGraph, wildcardNodeID, aCommandTracker);

			Node& createdNode = GetNode(createdNodeID, aNodeGraph);
			Node& replacedNode = GetNode(wildcardNodeID, aNodeGraph);
			SetNodePosition(createdNodeID, replacedNode.GetPosition(), aNodeGraph, nullptr);

			{ // Link new pin
				const size_t pinIndex = GetPinIndex(aNodeGraph, aWildcardPinID);

				const PinID createdPinConnectedID = SelectByIODirection(wildcardPinType.GetIODirection(), createdNode.GetInputPins()[pinIndex], createdNode.GetOutputPins()[pinIndex]);

				TryCreateLink(aNodeGraph, aConnectedPinID, createdPinConnectedID, aCommandTracker);
			}

			{ // Link previously linked pins


				for (size_t pinIndex = 0; pinIndex < replacedNode.GetInputPins().size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = aNodeGraph.GetPin(replacedNode.GetInputPins()[pinIndex]);

					if (!destroyedInputPin.GetConnectedPinIDs().empty())
					{
						TryCreateLink(aNodeGraph, destroyedInputPin.GetConnectedPinIDs()[0], GetPinID(aNodeGraph, createdNodeID, pinIndex, eIODirection::Input), aCommandTracker);
					}

				}

				for (size_t pinIndex = 0; pinIndex < replacedNode.GetOutputPins().size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = aNodeGraph.GetPin(replacedNode.GetOutputPins()[pinIndex]);

					for (const PinID connectedInputPinID : destroyedOutputPin.GetConnectedPinIDs())
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							TryCreateLink(aNodeGraph, connectedInputPinID, GetPinID(aNodeGraph, createdNodeID, pinIndex, eIODirection::Output), aCommandTracker);
						}
					}
				}
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

		}

		void ReplaceTemplateNode(NodeGraph& aNodeGraph, const NodeID aReplacedNodeID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeType& replacedNodeType = GetNodeTypeManager().GetNodeType(aNodeGraph.GetNode(aReplacedNodeID).GetTypeID());

			if (replacedNodeType.GetOperatorTrait() == eNodeOperatorTrait::None)
			{
				return;
			}

			//const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&aDataTypeID.mID);

			/*if (!dataTypeID)
			{
				return;
			}*/

			const bool canReplace = GetNodeTypeManager().CanCreateOperatorNode(replacedNodeType.GetOperatorTrait(), aDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, replacedNodeType.GetOperatorTrait(), aDataTypeID, aCommandTracker);
			const Node& replacedNode = GetNode(aReplacedNodeID, aNodeGraph);
			SetNodePosition(createdNodeID, replacedNode.GetPosition(), aNodeGraph, aCommandTracker);
			DestroyNode(aNodeGraph, aReplacedNodeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, const bool aIncludeDestroyed)
		{
			std::vector<PinID> pinIDs;
			pinIDs.reserve(aNodeGraph.GetPinCount());

			for (PinID i{ 0 }; i < aNodeGraph.GetPinCount(); i++)
			{
				const Pin& pin = aNodeGraph.GetPin(i);
				if (GetPinTypeManager().GetPinType(pin.GetTypeID()).GetIODirection() != eIODirection::Input)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.GetNode(pin.GetNodeID()).IsDestroyed())
				{
					continue;
				}

				pinIDs.push_back(i);
			}
			return pinIDs;
		}

		std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, const bool aIncludeDestroyed)
		{
			std::vector<PinID> pinIDs;
			pinIDs.reserve(aNodeGraph.GetPinCount());

			for (PinID i{ 0 }; i < aNodeGraph.GetPinCount(); i++)
			{
				const Pin& pin = aNodeGraph.GetPin(i);
				if (GetPinTypeManager().GetPinType(pin.GetTypeID()).GetIODirection() != eIODirection::Output)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.GetNode(pin.GetNodeID()).IsDestroyed())
				{
					continue;
				}

				pinIDs.push_back(i);
			}
			return pinIDs;
		}

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef)
		{
			return GetFoundation().GetVariableRefByNodeRef(aNodeRef);
		}

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef)
		{
			return GetFoundation().GetNodeRefsByVariableRef(aVarRef);
		}

		PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eIODirection aPinFlowType)
		{
			const Node& node = aNodeGraph.GetNode(aNodeID);
			const auto& pins = SelectByIODirection(aPinFlowType, node.GetInputPins(), node.GetOutputPins());

			return pins[aPinIndex];
		}

		size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID)
		{
			const Pin& pin = aNodeGraph.GetPin(aPinID);
			const PinType& pinType = GetPinTypeManager().GetPinType(pin.GetTypeID());
			const Node& node = aNodeGraph.GetNode(pin.GetNodeID());

			const std::vector<PinID>& pinIDs = SelectByIODirection(pinType.GetIODirection(), node.GetInputPins(), node.GetOutputPins());

			auto it = std::find(begin(pinIDs), end(pinIDs), aPinID);
			if (it != end(pinIDs))
			{
				return std::distance(begin(pinIDs), it);
			}

			assert(false);
			return InvalidID<size_t>();
		}

		PinID GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID)
		{
			const size_t pinIndex = GetPinIndex(aPreviousNodeGraph, aPreviousPinID);
			const PinType& pinType = GetPinType(aPreviousPinID, aPreviousNodeGraph);
			return GetPinID(aNewNodeGraph, aNodeID, pinIndex, pinType.GetIODirection());
		}

		bool AreDataTypesLinkable(const GenericDataTypeID aInputDataTypeID, const GenericDataTypeID aOutputDataTypeID)
		{
			const eDataTypeRelation dataTypeRelation = GetDataTypeManager().GetDataTypeRelation(aInputDataTypeID, aOutputDataTypeID);

			switch (dataTypeRelation)
			{
			case eDataTypeRelation::None:
				return false;
			case eDataTypeRelation::Same:
				return true;
			case eDataTypeRelation::Pointer_Value:
				return true;
			case eDataTypeRelation::Value_Pointer:
				return true;
				break;
			default:
				break;
			}

			return false;
		}

		bool AreDataTypesLinkable(const DataTypeID aInputDataTypeID, const DataTypeID aOutputDataTypeID)
		{
			const eDataTypeRelation dataTypeRelation = GetDataTypeManager().GetDataTypeRelation(aInputDataTypeID, aOutputDataTypeID);

			switch (dataTypeRelation)
			{
			case eDataTypeRelation::None:
				return false;
			case eDataTypeRelation::Same:
				return true;
			case eDataTypeRelation::Pointer_Value:
				return true;
			case eDataTypeRelation::Value_Pointer:
				return true;
				break;
			default:
				break;
			}

			return false;
		}

		bool ArePinTypesLinkableByDataType(const PinTypeID aInputPinTypeID, const PinTypeID aOutputPinTypeID)
		{
			const PinType& inputPinType = GetPinTypeManager().GetPinType(aInputPinTypeID);
			const PinType& outputPinType = GetPinTypeManager().GetPinType(aOutputPinTypeID);

			return AreDataTypesLinkable(inputPinType.GetDataTypeID(), outputPinType.GetDataTypeID());
		}

		static bool ArePinsLinkableByDataType(const NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID)
		{
			return ArePinTypesLinkableByDataType(GetPin(aInputPinID, aNodeGraph).GetTypeID(), GetPin(aOutputPinID, aNodeGraph).GetTypeID());
		}

		Link ArePinsLinkable(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2)
		{
			const Pin& pin1 = aNodeGraph.GetPin(aPinID1);
			const Pin& pin2 = aNodeGraph.GetPin(aPinID2);
			const PinType& pinType1 = GetPinTypeManager().GetPinType(pin1.GetTypeID());
			const PinType& pinType2 = GetPinTypeManager().GetPinType(pin2.GetTypeID());

			PinID inputPinID;
			PinID outputPinID;

			switch (pinType1.GetIODirection())
			{
			case eIODirection::Input:
				if (pinType2.GetIODirection() == eIODirection::Output)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
					{
						inputPinID = aPinID1;
						outputPinID = aPinID2;
					}
				}
				break;
			case eIODirection::Output:
				if (pinType2.GetIODirection() == eIODirection::Input)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID2, aPinID1))
					{
						inputPinID = aPinID2;
						outputPinID = aPinID1;
					}
				}
				break;
			default:
				break;
			}
			return Link(inputPinID, outputPinID);
		}

		LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed)
		{
			for (LinkID id{ 0 }; id < aNodeGraph.GetLinkCount(); ++id)
			{
				const Link& link = aNodeGraph.GetLink(id);
				if (!aIncludeDestroyed && link.IsDestroyed())
				{
					continue;
				}
				if (link == Link{ aPinID1, aPinID2 } || link == Link{ aPinID2, aPinID1 })
				{
					return id;
				}
			}
			return InvalidID<LinkID>();
		}

		std::vector<LinkID> GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed)
		{
			std::vector<LinkID> linkIDs;
			const Pin& pin = aNodeGraph.GetPin(aPinID);

			for (const PinID connectedPinID : pin.GetConnectedPinIDs())
			{
				const LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID, aIncludeDestroyed);
				assert(linkID != InvalidID<LinkID>());

				linkIDs.push_back(linkID);
			}

			return linkIDs;
		}

		std::vector<LinkID> GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID)
		{
			std::vector<LinkID> linkIDs;
			const Node& node = aNodeGraph.GetNode(aNodeID);

			for (const PinID inputPinID : node.GetInputPins())
			{
				std::vector<LinkID> inputLinks = GetLinkIDsByPin(aNodeGraph, inputPinID);
				linkIDs.insert(linkIDs.end(), inputLinks.begin(), inputLinks.end());
			}
			for (const PinID mOutputPinID : node.GetOutputPins())
			{
				std::vector<LinkID> outputLinks = GetLinkIDsByPin(aNodeGraph, mOutputPinID);
				linkIDs.insert(linkIDs.end(), outputLinks.begin(), outputLinks.end());
			}


			return linkIDs;
		}
	}
}
