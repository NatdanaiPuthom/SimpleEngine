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
			return GetFoundation().mDataTypeManager;
		}

		NodeTypeManager& GetNodeTypeManager()
		{
			return GetFoundation().mNodeTypeManager;
		}

		PinTypeManager& GetPinTypeManager()
		{
			return GetFoundation().mPinTypeManager;
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
			return GetFoundation().mMemoryPool;
		}

		EventGraph& GetNodeGraphCopy()
		{
			return GetFoundation().mNodeGraphCopy;
		}
	}

	struct PinRef
	{
		NodeGraph* mNodeGraph = nullptr;
		PinID mPinID = InvalidID<PinID>();

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
					return aEventGraph->mNodeGraph;
				},
				[](FunctionIDWrapper aFunctionIDWrapper) -> NodeGraph&
				{
					return GetNodeTypeManager().GetFunction(aFunctionIDWrapper.mID).mNodeGraph;
				}
				}, aNodeGraphVariant);
		}

		const Pin& GetPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		Pin& GetPin(const PinID aPinID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		const PinType& GetPinType(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			const Pin& pin = GetPin(aPinID, aNodeGraph);
			return GetPinTypeManager().GetPinType(pin.mTypeID);
		}

		const PinType& GetPinType(const Pin& aPin)
		{
			return GetPinTypeManager().GetPinType(aPin.mTypeID);
		}

		Node& GetNode(const NodeID aNodeID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		const Node& GetNode(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		const NodeType& GetNodeType(const Node& aNode)
		{
			return GetNodeTypeManager().GetNodeType(aNode.mTypeID);
		}

		const NodeType& GetNodeType(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			const Node& node = GetNode(aNodeID, aNodeGraph);
			return GetNodeTypeManager().GetNodeType(node.mTypeID);
		}

		const DataType* GetDataTypeByID(const DataTypeID aDataTypeID)
		{
			return GetDataTypeManager().Find(aDataTypeID);
		}

		Class& GetClassByID(const ClassID aClassID)
		{
			return GetDataTypeManager().GetClass(aClassID);
		}

		Struct& GetStructByID(const StructID aStructID)
		{
			return GetDataTypeManager().GetStruct(aStructID);
		}

		void InitializeSubPinsRecursivelyGeneric(const eFlowType aFlowType, const std::vector<PinTypeID>& aPinTypeIDs)
		{
			PinTypeManager& pinTypeManager = GetPinTypeManager();
			const DataTypeManager& dataTypeManager = GetDataTypeManager();

			for (const PinTypeID pinTypeID : aPinTypeIDs)
			{
				if (const DataType* dataType = dataTypeManager.Find(pinTypeManager.GetPinType(pinTypeID).mDataTypeID))
				{
					for (const Variable& variable : dataType->mVariables)
					{
						if (const DataType* dataType2 = dataTypeManager.Find(variable.mDataTypeID))
						{
							const PinTypeID newSubPinTypeID = pinTypeManager.CreatePinType(variable.mName, aFlowType, variable.mDataTypeID, dataTypeManager.GetSetPinValueInterface(variable.mDataTypeID, aFlowType), dataTypeManager.GetSetPinValueFromPinInterface(variable.mDataTypeID, aFlowType));

							pinTypeManager.GetPinType(pinTypeID).mSubPinTypeIDs.push_back(newSubPinTypeID);
						}
					}
					InitializeSubPinsRecursivelyGeneric(aFlowType, pinTypeManager.GetPinType(pinTypeID).mSubPinTypeIDs);
				}

			}
		}

		void InitializeSubPinsRecursively(const eFlowType aFlowType, const std::vector<PinTypeID>& aPinTypeIDs)
		{
			PinTypeManager& pinTypeManager = GetPinTypeManager();
			const DataTypeManager& dataTypeManager = GetDataTypeManager();

			for (const PinTypeID pinTypeID : aPinTypeIDs)
			{
				if (const DataType* dataType = dataTypeManager.Find(pinTypeManager.GetPinType(pinTypeID).mDataTypeID))
				{
					for (const Variable& variable : dataType->mVariables)
					{
						if (const DataType* dataType2 = dataTypeManager.Find(variable.mDataTypeID))
						{
							const PinTypeID newSubPinTypeID = pinTypeManager.CreatePinType(variable.mName, aFlowType, variable.mDataTypeID, dataTypeManager.GetSetPinValueInterface(variable.mDataTypeID, aFlowType), dataTypeManager.GetSetPinValueFromPinInterface(variable.mDataTypeID, aFlowType));

							pinTypeManager.GetPinType(pinTypeID).mSubPinTypeIDs.push_back(newSubPinTypeID);
						}
					}
					InitializeSubPinsRecursively(aFlowType, pinTypeManager.GetPinType(pinTypeID).mSubPinTypeIDs);
				}

			}
		}

		void InitializeSubPins()
		{
			const std::vector<NodeType>& nodeTypes = GetNodeTypeManager().GetNodeTypes();

			for (const NodeType& nodeType : nodeTypes)
			{
				InitializeSubPinsRecursively(eFlowType::Input, nodeType.mNodeRecipe.mInputPinTypeIDs);
				InitializeSubPinsRecursively(eFlowType::Output, nodeType.mNodeRecipe.mOutputPinTypeIDs);
			}
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

		StructID CreateStruct(std::string_view aName)
		{
			return GetDataTypeManager().CreateStruct(aName);
		}

		void SetStructName(const StructID aStructID, std::string_view aName, CommandTracker* const aCommandTracker)
		{
			struct SetStructNameData final
			{
				StructID mStructID = InvalidID<StructID>();
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
		}

		ClassID CreateClass(const DataTypeID aTargetID, const std::string_view aName)
		{
			return GetDataTypeManager().CreateClass(aTargetID, aName);
		}

		void SetClassName(const ClassID aClassID, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			struct SetClassNameData final
			{
				ClassID mClassID = InvalidID<ClassID>();
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

		CustomEventID CreateCustomEvent(const std::string_view aName)
		{
			return GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(const std::string_view aName)
		{
			const FunctionID id = GetNodeTypeManager().CreateFunction(aName);

			Function& createdFunction = GetNodeTypeManager().GetFunction(id);

			createdFunction.mInputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.mInputNodeTypeID);
			createdFunction.mOutputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.mOutputNodeTypeID);

			return id;
		}

		NodeID CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Position");
			}

			NodeGraph& nodeGraph = GetNodeGraph(aNodeGraphVariant);
			const NodeID nodeID = GetCurrentNodeID(nodeGraph);
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


		NodeID CreateNode(const NodeGraphVariantHandle& aNodeGraphVariant, const std::string_view aName, bool& aSuccess, const Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* const aCommandTracker)
		{
			const NodeTypeID typeID = GetNodeTypeManager().GetTypeID(aName);
			aSuccess = typeID != NodeTypeID{ 0 };
			if (!aCreateIfNameNotFound && !aSuccess)
			{
				return InvalidID<NodeID>();
			}
			return CreateNode(aNodeGraphVariant, typeID, aPosition, aCommandTracker);
		}

		NodeID CreateNodeAutoLink(const NodeGraphVariantHandle& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Auto Link");
			}

			const NodeID createdNodeID = Internal::CreateNode(aNodeGraphVariant, aNodeTypeID, aPosition, aCommandTracker);

			const Pin& createdFromPin = GetNodeGraph(aNodeGraphVariant).mPins.at(aConnection);
			const PinType& pinType = GetPinTypeManager().GetPinType(createdFromPin.mTypeID);
			const Node& createdNode = GetNodeGraph(aNodeGraphVariant).mNodes.at(createdNodeID);

			const std::vector<PinID>& pinIDs = SelectByFlowType(pinType.mFlowType, createdNode.mOutputPins, createdNode.mInputPins);

			for (const PinID pinID : pinIDs)
			{
				if (Internal::TryCreateLink(GetNodeGraph(aNodeGraphVariant), pinID, aConnection, aCommandTracker) != InvalidID<LinkID>())
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

			const NodeID nodeID = GetCurrentNodeID(aNodeGraph);
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

			const NodeID nodeID = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, GetNodeTypeManager().CreateSetterNode(aNodeGraph, nodeID, aDataTypeID), nodeID, aCommandTracker);


			Internal::SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			Internal::BindVariable(aClass, CreateContextualNodeRef(nodeID, aNodeGraph), aVarID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;


		}

		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, GetNodeTypeManager().CreateOperatorNode(aNodeGraph, id, aOperatorTrait, aDataTypeID), id, aCommandTracker);
			return id;
		}

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* const aCommandTracker)
		{
			std::vector<Node>& nodes = aNodeGraph.mNodes;

			nodes.emplace_back(std::move(aNode));

			Node& createdNode = nodes.back();

			NodeType& nodeType = GetNodeTypeManager().GetNodeType(createdNode.mTypeID);
			nodeType.mNodeRefs.push_back(CreateContextualNodeRef(aNodeID, aNodeGraph));

			struct CreateNodeData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;

			auto doCommandFunction = [](const auto& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.mIsDestroyed = false;
				};

			auto undoCommandFunction = [](const auto& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.mIsDestroyed = true;
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
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.mIsDestroyed = true;
				};

			auto undoCommandFunction = [](const auto& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.mIsDestroyed = false;
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

		void DestroyNodes(const std::vector<GlobalNodeRef>& aNodeRefs, CommandTracker* aCommandTracker)
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

		void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* const aCommandTracker)
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

		void DestroyNodes(const std::vector<NodeID>& aNodeIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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

		void DestroyLinks(const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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
			const Vec2 oldPos = aNodeGraph.mNodes.at(aNodeID).mPosition;
			SetNodePosition(aNodeID, aPosition, oldPos, aNodeGraph, aCommandTracker);
		}

		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{

			struct SetNodePositionData final
			{
				NodeID mNodeID = InvalidID<NodeID>();
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
					node.mPosition = aData.mNewPos;
				};

			auto undoCommandFunction = [](const SetNodePositionData& aData) -> void
				{
					Node& node = GetNode(aData.mNodeID, *aData.mNodeGraph);
					node.mPosition = aData.mOldPos;
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

		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
		{
			const NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.mNodeRecipe.mInputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}

		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
		{
			const NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.mNodeRecipe.mOutputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
		{
			const auto dataTypeID = GetPinTypeManager().GetPinType(aPinTypeID).mDataTypeID;

			void* const dataPtr = GetDataTypeManager().AllocateData(dataTypeID, aNodeGraph.mMemoryArena);

			return CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
		{
			const PinID createdPinID = PinID{ static_cast<PinID::id_type>(aNodeGraph.mPins.size()) };

			const PinType& pinType = GetPinTypeManager().GetPinType(aPinTypeID);

			aNodeGraph.mPins.push_back(Pin{ .mTypeID = aPinTypeID, .mDataPtr = aDataPtr, .mNodeID = aNodeID });

			std::vector<PinID> subPinIDs;
			subPinIDs.reserve(pinType.mSubPinTypeIDs.size());
			const DataType* dataType = GetDataTypeManager().Find(pinType.mDataTypeID);

			GenericDataTypePtr d = GetDataTypeManager().Find(pinType.mGenericDataTypeID);

			/*const std::vector<Variable>& variables = std::visit(Visitor{
				[](const DataType* aDataType) -> const std::vector<Variable>& { return aDataType->mVariables; },
				[](const Struct* aDataType) -> const std::vector<Variable>& { return aDataType->mVariableContainer.mVariables; },
				[](const Class* aDataType) -> const std::vector<Variable>& { return aDataType->mVariableContainer.mVariables; }
				}, d);*/

			assert(pinType.mSubPinTypeIDs.size() == dataType->mVariables.size());
			for (size_t i = 0; i < pinType.mSubPinTypeIDs.size(); i++)
			{
				void* const dataPtr = reinterpret_cast<char*>(aDataPtr) + dataType->mVariables[i].mByteOffset;

				const PinID createdSubPinID = CreatePin(aNodeGraph, aNodeID, pinType.mSubPinTypeIDs[i], dataPtr);
				subPinIDs.push_back(createdSubPinID);

				GetPin(createdSubPinID, aNodeGraph).mParentPinID = createdPinID;
			}

			Pin& createdPin = GetPin(createdPinID, aNodeGraph);
			createdPin.mSubPinIDs = std::move(subPinIDs);

			return createdPinID;
		}

		void ViewAndEditPinGeneric(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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
					return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.mGenericDataTypeID, Internal::GetFrameMemoryArena(), pin.mDataPtr) : nullptr;
				}();

			const ViewAndEditResult viewAndEditResult = dataTypeManager.ViewAndEditData(pinType.mGenericDataTypeID, pin.mDataPtr);

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

					GetDataTypeManager().SwapData(pinType.mGenericDataTypeID, pin.mDataPtr, aData.mPreviousDataPtr);
				};

			auto undoCommandFunction = [](const EditPinData& aData) -> void
				{
					Pin& pin = GetPin(aData.mPinID, *aData.mNodeGraph);
					const PinType& pinType = GetPinType(pin);

					GetDataTypeManager().SwapData(pinType.mGenericDataTypeID, pin.mDataPtr, aData.mPreviousDataPtr);
				};

			gChangePinValueCommand = Command(data, doCommandFunction, undoCommandFunction, "Edit Pin");
		}


		void ViewAndEditPin(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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
				PinID mPinID = InvalidID<PinID>();
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
		}

		void ViewPinGeneric(PinID aPinID, const NodeGraph& aNodeGraph)
		{
			GetDataTypeManager().ViewData(GetPinType(aPinID, aNodeGraph).mGenericDataTypeID, GetPin(aPinID, aNodeGraph).mDataPtr);
		}

		void ViewPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			GetDataTypeManager().ViewData(GetPinType(aPinID, aNodeGraph).mDataTypeID, GetPin(aPinID, aNodeGraph).mDataPtr);
		}

		static void SplitPinInternal(const PinID aPinID, NodeGraph& aNodeGraph, const size_t aIndex)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);
			pin.mIsSplit = true;
			Node& node = GetNode(pin.mNodeID, aNodeGraph);
			const PinType& pinType = GetPinType(aPinID, aNodeGraph);
			auto& nodePins = SelectByFlowType(pinType.mFlowType, node.mSplitInputPins, node.mSplitOutputPins);

			nodePins.erase(nodePins.begin() + aIndex);
			nodePins.insert(nodePins.begin() + aIndex, pin.mSubPinIDs.begin(), pin.mSubPinIDs.end());
		}

		static void RecombinePinInternal(const PinID aPinID, NodeGraph& aNodeGraph, const size_t aIndex)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);
			pin.mIsSplit = false;
			Node& node = GetNode(pin.mNodeID, aNodeGraph);
			const PinType& pinType = GetPinType(aPinID, aNodeGraph);
			auto& nodePins = SelectByFlowType(pinType.mFlowType, node.mSplitInputPins, node.mSplitOutputPins);

			nodePins.erase(nodePins.begin() + aIndex, nodePins.begin() + aIndex + pin.mSubPinIDs.size());
			nodePins.insert(nodePins.begin() + aIndex, aPinID);
		}

		void SplitPin(const PinID aPinID, NodeGraph& aNodeGraph, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			Pin& pin = GetPin(aPinID, aNodeGraph);

			if (!pin.mConnectedPinIDs.empty())
			{
				assert(false);
				return;
			}

			Node& node = GetNode(pin.mNodeID, aNodeGraph);

			const PinType& pinType = GetPinType(aPinID, aNodeGraph);

			auto& nodePins = SelectByFlowType(pinType.mFlowType, node.mSplitInputPins, node.mSplitOutputPins);

			auto it = std::find(nodePins.begin(), nodePins.end(), aPinID);
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

			data.mIndex = std::distance(nodePins.begin(), it);
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

			if (!pin.mConnectedPinIDs.empty())
			{
				assert(false);
				return;
			}

			Node& node = GetNode(pin.mNodeID, aNodeGraph);

			const PinType& pinType = GetPinType(aPinID, aNodeGraph);

			const auto& nodePins = SelectByFlowType(pinType.mFlowType, node.mSplitInputPins, node.mSplitOutputPins);

			auto it = std::find(nodePins.begin(), nodePins.end(), pin.mSubPinIDs.front());
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

			return nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None;
		}

		void ActivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.mLinks[aLinkID];


			{
				Pin& inputPin = aNodeGraph.mPins.at(link.mInputPinID);
				auto it = std::find(inputPin.mConnectedPinIDs.begin(), inputPin.mConnectedPinIDs.end(), link.mOutputPinID);
				if (it == inputPin.mConnectedPinIDs.end())
				{
					inputPin.mConnectedPinIDs.push_back(link.mOutputPinID);
				}
			}
			{
				Pin& outputPin = aNodeGraph.mPins.at(link.mOutputPinID);
				auto it = std::find(outputPin.mConnectedPinIDs.begin(), outputPin.mConnectedPinIDs.end(), link.mInputPinID);
				if (it == outputPin.mConnectedPinIDs.end())
				{
					outputPin.mConnectedPinIDs.push_back(link.mInputPinID);
				}
			}

			link.mIsDestroyed = false;
		}

		void DeactivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.mLinks[aLinkID];

			Pin& inputPin = aNodeGraph.mPins.at(link.mInputPinID);
			Pin& outputPin = aNodeGraph.mPins.at(link.mOutputPinID);

			std::erase(inputPin.mConnectedPinIDs, link.mOutputPinID);
			std::erase(outputPin.mConnectedPinIDs, link.mInputPinID);

			link.mIsDestroyed = true;
		}

		LinkID TryCreateLink(NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, CommandTracker* const aCommandTracker)
		{
			const Link createdLink = ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded operator node
				const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
				const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);

				const PinType& pinType1 = GetPinTypeManager().GetPinType(pin1.mTypeID);
				const PinType& pinType2 = GetPinTypeManager().GetPinType(pin2.mTypeID);

				if (pinType1.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID1, aPinID2, aCommandTracker);
				}
				else if (pinType2.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
				}

				return InvalidID<LinkID>();
			}

			return CreateLink(aNodeGraph, createdLink.mInputPinID, createdLink.mOutputPinID, aCommandTracker);
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

			data.mNodeGraph = &aNodeGraph;
			data.mCreatedLinkID = LinkID{ static_cast<LinkID::id_type>(aNodeGraph.mLinks.size()) };

			const Pin& inputPin = aNodeGraph.mPins.at(aInputPinID);
			const Pin& outputPin = aNodeGraph.mPins.at(aOutputPinID);
			const PinType& inputPinType = GetPinTypeManager().GetPinType(inputPin.mTypeID);
			const PinType& outputPinType = GetPinTypeManager().GetPinType(outputPin.mTypeID);
			assert(inputPinType.mFlowType == eFlowType::Input);
			assert(outputPinType.mFlowType == eFlowType::Output);
			assert(GetDataTypeManager().AreDataTypesRelated(inputPinType.mGenericDataTypeID, outputPinType.mGenericDataTypeID));

			if (inputPinType.mGenericDataTypeID != GenericDataTypeID{ GetDataTypeID<Flow>() })
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

			aNodeGraph.mLinks.push_back(Link{ aInputPinID, aOutputPinID });

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

			data.mNodeGraph = &aNodeGraph;
			data.mCreatedLinkID = LinkID{ static_cast<LinkID::id_type>(aNodeGraph.mLinks.size()) };

			const Pin& inputPin = aNodeGraph.mPins.at(aInputPinID);
			const Pin& outputPin = aNodeGraph.mPins.at(aOutputPinID);
			const PinType& inputPinType = GetPinTypeManager().GetPinType(inputPin.mTypeID);
			const PinType& outputPinType = GetPinTypeManager().GetPinType(outputPin.mTypeID);
			const DataTypeID inputPinDataType = inputPinType.mDataTypeID;
			const DataTypeID outputPinDataType = outputPinType.mDataTypeID;
			assert(inputPinType.mFlowType == eFlowType::Input);
			assert(outputPinType.mFlowType == eFlowType::Output);
			assert(GetDataTypeManager().AreDataTypesRelated(inputPinDataType, outputPinDataType));

			if (inputPinDataType != GetDataTypeID<Flow>())
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

			aNodeGraph.mLinks.push_back(Link{ aInputPinID, aOutputPinID });

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
			const Pin& pin = aNodeGraph.mPins.at(aPinID);

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Links By Pin");
			}


			for (const PinID connectedPinID : pin.mConnectedPinIDs)
			{
				const LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID);
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		VarID CreateVariable(VariableContainer& aVariableContainer, const DataTypeID aDataTypeID, const std::string_view aName, CommandTracker* const aCommandTracker)
		{
			std::vector<Variable>& variables = aVariableContainer.mVariables;
			const VarID varID{ variables.size() };
			variables.emplace_back();

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

		void SetVariableDataType(const VarID aVarID, VariableContainer& aVariableContainer, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			Variable& variable = aVariableContainer.mVariables.at(aVarID);

			void* const defaultValueDataPtr = GetDataTypeManager().AllocateData(aDataTypeID, aVariableContainer.mMemoryArena);

			variable.mDataTypeID = aDataTypeID;
			variable.mDefaultValueDataPtr = defaultValueDataPtr;

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

			data.mOldName = aVariableContainer.mVariables[aVarID].mName;
			data.mNewName = aName;
			data.mVarID = aVarID;
			data.mVariableContainer = &aVariableContainer;

			auto doCommandFunction = [](const SetVariableNameData& aData) -> void
				{
					aData.mVariableContainer->mVariables.at(aData.mVarID).mName = aData.mNewName;
				};

			auto undoCommandFunction = [](const SetVariableNameData& aData) -> void
				{
					aData.mVariableContainer->mVariables.at(aData.mVarID).mName = aData.mOldName;
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
					aData.mVariableContainer->mVariables.at(aData.mVarID).mIsDestroyed = true;
				};


			auto undoCommandFunction = [](const DestroyVariableData& aData) -> void
				{
					aData.mVariableContainer->mVariables.at(aData.mVarID).mIsDestroyed = false;
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
			Variable& variable = aVariableContainer.mVariables.at(aVarID);

			ViewAndEditResult r = GetDataTypeManager().ViewAndEditData(variable.mDataTypeID, variable.mDefaultValueDataPtr);
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

			pinType.mName = aName;
		}

		static PinTypeID AddPinToNodeType(const NodeTypeID aNodeTypeID, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName)
		{
			NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);

			const PinTypeID createdPinTypeID = GetPinTypeManager().CreatePinType(aPinName, aFlowType, aDataTypeID, GetDataTypeManager().GetSetPinValueInterface(aDataTypeID, aFlowType), GetDataTypeManager().GetSetPinValueFromPinInterface(aDataTypeID, aFlowType));

			std::vector<PinTypeID>& pinTypeIDs = aFlowType == eFlowType::Input ? nodeType.mNodeRecipe.mInputPinTypeIDs : nodeType.mNodeRecipe.mOutputPinTypeIDs;
			pinTypeIDs.push_back(createdPinTypeID);

			for (const NodeRef& nodeRef : nodeType.mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), createdPinTypeID);

				pinIDs.push_back(createdPinID);
			}

			return createdPinTypeID;
		}


		static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
		{
			NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			PinTypeManager& pinTypeManager = GetPinTypeManager();

			NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);
			const PinTypeID oldPinTypeID = pinTypeIDs.at(aIndex);
			const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

			const PinTypeID newPinTypeID = pinTypeManager.CreatePinType(oldPinType.mName, aFlowType, aDataTypeID,
				GetDataTypeManager().GetSetPinValueInterface(aDataTypeID, aFlowType),
				GetDataTypeManager().GetSetPinValueFromPinInterface(aDataTypeID, aFlowType));

			pinTypeIDs.at(aIndex) = newPinTypeID;

			const std::vector<NodeRef>& mNodeRefs = nodeType.mNodeRefs;
			for (const NodeRef& nodeRef : mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), newPinTypeID);
				pinIDs.at(aIndex) = createdPinID;
			}
		}

		static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const eFlowType aFlowType)
		{
			NodeType& nodeType = GetNodeTypeManager().GetNodeType(aNodeTypeID);

			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

			assert(aIndex < pinTypeIDs.size());

			pinTypeIDs.erase(pinTypeIDs.begin() + aIndex);

			for (const NodeRef& nodeRef : nodeType.mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				DestroyLinksByPin(nodeRef.GetNodeGraph(), pinIDs.at(aIndex), nullptr);
				pinIDs.erase(pinIDs.begin() + aIndex);
			}
		}

		void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName, [[maybe_unused]] CommandTracker* aCommandTracker)
		{
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);
			NodeType& executorNodeType = GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
			NodeType& callerNodeType = GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

			const std::string nameDirectory = GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
			executorNodeType.mNodeRecipe.mName = nameDirectory + std::string(aName);
			callerNodeType.mNodeRecipe.mName = nameDirectory + "Call " + std::string(aName);
		}

		void AddPinToCustomEvent(const CustomEventID aCustomEventID, const DataTypeID aDataTypeID, const std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			AddPinToNodeType(customEvent.GetExecutorTypeID(), aDataTypeID, eFlowType::Output, aPinName);
			AddPinToNodeType(customEvent.GetCallerTypeID(), aDataTypeID, eFlowType::Input, aPinName);
		}

		void SetPinDataTypeAtIndexCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, aDataTypeID, eFlowType::Output);
			SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, aDataTypeID, eFlowType::Input);
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

				SetPinTypeName(callerNodeType.mNodeRecipe.mInputPinTypeIDs.at(aIndex), aName);
			}

			{
				const NodeType& executorNodeType = nodeTypeManager.GetNodeType(customEvent.GetExecutorTypeID());

				SetPinTypeName(executorNodeType.mNodeRecipe.mOutputPinTypeIDs.at(aIndex), aName);
			}
		}

		void DeletePinAtIndexCustomEvent(const CustomEventID aCustomEventID, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}

			const CustomEvent& customEvent = GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, eFlowType::Input);
			DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, eFlowType::Output);
		}

		void AddPinToFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);


			AddPinToNodeType(function.mCallerNodeTypeID, aDataTypeID, aFlowType, aPinName);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			AddPinToNodeType(inputOutputNodeTypeID, aDataTypeID, InvertFlowType(aFlowType), aPinName);
		}

		void SetPinDataTypeAtIndexFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = GetNodeTypeManager().GetFunction(aFunctionID);

			SetPinAtIndexNodeType(function.mCallerNodeTypeID, aIndex, aDataTypeID, aFlowType);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			SetPinAtIndexNodeType(inputOutputNodeTypeID, aIndex, aDataTypeID, InvertFlowType(aFlowType));
		}

		void SetPinNameAtIndexFunction(const FunctionID aFunctionID, const std::string_view aName, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);

			{
				const NodeType& callerNodeType = nodeTypeManager.GetNodeType(function.mCallerNodeTypeID);

				const std::vector<PinTypeID>& callerPinTypeIDs = SelectByFlowType(aFlowType, callerNodeType.mNodeRecipe.mInputPinTypeIDs, callerNodeType.mNodeRecipe.mOutputPinTypeIDs);
				SetPinTypeName(callerPinTypeIDs.at(aIndex), aName);
			}

			{
				const NodeType& inputNodeType = nodeTypeManager.GetNodeType(function.mInputNodeTypeID);
				const NodeType& outputNodeType = nodeTypeManager.GetNodeType(function.mOutputNodeTypeID);

				const std::vector<PinTypeID>& inputOutputPinTypeIDs = SelectByFlowType(aFlowType, inputNodeType.mNodeRecipe.mOutputPinTypeIDs, outputNodeType.mNodeRecipe.mInputPinTypeIDs);
				SetPinTypeName(inputOutputPinTypeIDs.at(aIndex), aName);
			}
		}

		void DeletePinAtIndexFunction(const FunctionID aFunctionID, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = GetNodeTypeManager().GetFunction(aFunctionID);

			DeletePinAtIndexNodeType(function.mCallerNodeTypeID, aIndex, aFlowType);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			DeletePinAtIndexNodeType(inputOutputNodeTypeID, aIndex, InvertFlowType(aFlowType));
		}

		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, const PinID aWildcardPinID, const PinID aConnectedPinID, CommandTracker* const aCommandTracker)
		{
			const Pin& wildcardPin = GetPin(aWildcardPinID, aNodeGraph);
			const Pin& connectedPin = GetPin(aConnectedPinID, aNodeGraph);

			const NodeID wildcardNodeID = wildcardPin.mNodeID;

			const NodeType& wildcardNodeType = GetNodeTypeManager().GetNodeType(aNodeGraph.mNodes.at(wildcardNodeID).mTypeID);

			if (wildcardNodeType.mNodeRecipe.mOperatorTrait == eNodeOperatorTrait::None)
			{
				return;
			}

			const PinType& wildcardPinType = GetPinTypeManager().GetPinType(wildcardPin.mTypeID);
			const PinType& connectedPinType = GetPinTypeManager().GetPinType(connectedPin.mTypeID);
			const bool canReplace = GetNodeTypeManager().CanCreateOperatorNode(wildcardNodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, wildcardNodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID, aCommandTracker);



			DestroyNode(aNodeGraph, wildcardNodeID, aCommandTracker);


			Node& createdNode = GetNode(createdNodeID, aNodeGraph);
			Node& replacedNode = GetNode(wildcardNodeID, aNodeGraph);
			SetNodePosition(createdNodeID, replacedNode.mPosition, aNodeGraph, nullptr);

			{ // Link new pin
				const size_t pinIndex = GetPinIndex(aNodeGraph, aWildcardPinID);

				const PinID createdPinConnectedID = SelectByFlowType(wildcardPinType.mFlowType, createdNode.mInputPins.at(pinIndex), createdNode.mOutputPins.at(pinIndex));

				TryCreateLink(aNodeGraph, aConnectedPinID, createdPinConnectedID, aCommandTracker);
			}

			{ // Link previously linked pins


				for (size_t pinIndex = 0; pinIndex < replacedNode.mInputPins.size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = aNodeGraph.mPins.at(replacedNode.mInputPins.at(pinIndex));

					if (!destroyedInputPin.mConnectedPinIDs.empty())
					{
						TryCreateLink(aNodeGraph, destroyedInputPin.mConnectedPinIDs[0], GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Input), aCommandTracker);
					}

				}

				for (size_t pinIndex = 0; pinIndex < replacedNode.mOutputPins.size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = aNodeGraph.mPins.at(replacedNode.mOutputPins.at(pinIndex));

					for (const PinID connectedInputPinID : destroyedOutputPin.mConnectedPinIDs)
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							TryCreateLink(aNodeGraph, connectedInputPinID, GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Output), aCommandTracker);
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
			const NodeType& replacedNodeType = GetNodeTypeManager().GetNodeType(aNodeGraph.mNodes.at(aReplacedNodeID).mTypeID);

			if (replacedNodeType.mNodeRecipe.mOperatorTrait == eNodeOperatorTrait::None)
			{
				return;
			}

			const bool canReplace = GetNodeTypeManager().CanCreateOperatorNode(replacedNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, replacedNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID, aCommandTracker);
			const Node& replacedNode = GetNode(aReplacedNodeID, aNodeGraph);
			SetNodePosition(createdNodeID, replacedNode.mPosition, aNodeGraph, aCommandTracker);
			DestroyNode(aNodeGraph, aReplacedNodeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph)
		{
			return NodeID{ static_cast<NodeID::id_type>(aNodeGraph.mNodes.size()) };
		}

		std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, const bool aIncludeDestroyed)
		{
			std::vector<PinID> pinIDs;
			pinIDs.reserve(aNodeGraph.mPins.size());

			for (PinID i{ 0 }; i < aNodeGraph.mPins.size(); i++)
			{
				const Pin& pin = aNodeGraph.mPins[i];
				if (GetPinTypeManager().GetPinType(pin.mTypeID).mFlowType != eFlowType::Input)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.mNodes[pin.mNodeID].mIsDestroyed)
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
			pinIDs.reserve(aNodeGraph.mPins.size());

			for (PinID i{ 0 }; i < aNodeGraph.mPins.size(); i++)
			{
				const Pin& pin = aNodeGraph.mPins[i];
				if (GetPinTypeManager().GetPinType(pin.mTypeID).mFlowType != eFlowType::Output)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.mNodes[pin.mNodeID].mIsDestroyed)
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

		PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType)
		{
			const Node& node = aNodeGraph.mNodes.at(aNodeID);
			switch (aPinFlowType)
			{
			case eFlowType::Input:
				if (aPinIndex < node.mInputPins.size())
				{
					return node.mInputPins.at(aPinIndex);
				}
				break;
			case eFlowType::Output:
				if (aPinIndex < node.mOutputPins.size())
				{
					return node.mOutputPins.at(aPinIndex);
				}
				break;
			default:
				break;
			}
			return InvalidID<PinID>();
		}

		size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID)
		{
			const Pin& pin = aNodeGraph.mPins.at(aPinID);
			const PinType& pinType = GetPinTypeManager().GetPinType(pin.mTypeID);
			const Node& node = aNodeGraph.mNodes.at(pin.mNodeID);

			const std::vector<PinID>& pinIDs = SelectByFlowType(pinType.mFlowType, node.mInputPins, node.mOutputPins);

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
			return GetPinID(aNewNodeGraph, aNodeID, pinIndex, pinType.mFlowType);
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

			return AreDataTypesLinkable(inputPinType.mDataTypeID, outputPinType.mDataTypeID);
		}

		static bool ArePinsLinkableByDataType(const NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID)
		{
			return ArePinTypesLinkableByDataType(GetPin(aInputPinID, aNodeGraph).mTypeID, GetPin(aOutputPinID, aNodeGraph).mTypeID);
		}

		Link ArePinsLinkable(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2)
		{
			const Pin& pin1 = aNodeGraph.mPins[aPinID1];
			const Pin& pin2 = aNodeGraph.mPins[aPinID2];
			const PinType& pinType1 = GetPinTypeManager().GetPinType(pin1.mTypeID);
			const PinType& pinType2 = GetPinTypeManager().GetPinType(pin2.mTypeID);

			Link outputLink;

			switch (pinType1.mFlowType)
			{
			case eFlowType::Input:
				if (pinType2.mFlowType == eFlowType::Output)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
					{
						outputLink.mInputPinID = aPinID1;
						outputLink.mOutputPinID = aPinID2;
					}
				}
				break;
			case eFlowType::Output:
				if (pinType2.mFlowType == eFlowType::Input)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID2, aPinID1))
					{
						outputLink.mInputPinID = aPinID2;
						outputLink.mOutputPinID = aPinID1;
					}
				}
				break;
			default:
				break;
			}
			return outputLink;
		}

		LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed)
		{
			for (LinkID id{ 0 }; id < aNodeGraph.mLinks.size(); ++id)
			{
				const Link& link = aNodeGraph.mLinks[id];
				if (!aIncludeDestroyed && link.mIsDestroyed)
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
			const Pin& pin = aNodeGraph.mPins.at(aPinID);

			for (const PinID connectedPinID : pin.mConnectedPinIDs)
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
			const Node& node = aNodeGraph.mNodes.at(aNodeID);

			for (const PinID inputPinID : node.mInputPins)
			{
				std::vector<LinkID> inputLinks = GetLinkIDsByPin(aNodeGraph, inputPinID);
				linkIDs.insert(linkIDs.end(), inputLinks.begin(), inputLinks.end());
			}
			for (const PinID mOutputPinID : node.mOutputPins)
			{
				std::vector<LinkID> outputLinks = GetLinkIDsByPin(aNodeGraph, mOutputPinID);
				linkIDs.insert(linkIDs.end(), outputLinks.begin(), outputLinks.end());
			}


			return linkIDs;
		}
	}
}
