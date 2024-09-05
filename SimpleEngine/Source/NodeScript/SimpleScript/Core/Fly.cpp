#include "Fly.hpp"
#include "FlyClass.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"
#include "FlyInternal.hpp"
#include "Utilities/FlyLinker.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "FlyFoundation.hpp"
#include "Serialization/FlySerializer.hpp"

namespace FLY_NAMESPACE
{

	namespace Internal
	{

		static const Pin& GetPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		static Pin& GetPin(const PinID aPinID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		PinType& GetPinType(const Pin& aPin)
		{
			return Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
		}

		static Node& GetNode(const NodeID aNodeID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		const Node& GetNode(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		static const NodeType& GetNodeType(const Node& aNode)
		{
			return Global::GetNodeTypeManager().GetNodeType(aNode.mTypeID);
		}
	}

	void SaveClass(const ClassView aClassView, const std::string_view aSavePath)
	{
		ScriptLoader::SaveClass(aClassView.GetClass(), aSavePath);
	}

	void LoadAllClasses(const std::string_view aFilePath)
	{
		ScriptLoader::LoadAllClasses(aFilePath);
	}

	void SaveCustomEvents(const std::string_view aFilePath)
	{
		ScriptLoader::SaveCustomEvents(aFilePath);
	}

	ClassView CreateClass(const DataTypeView aTargetView, const std::string_view aName, const std::string_view aSavePath)
	{
		Class& createdClass = Global::GetFoundation().CreateClass(aTargetView.GetID(), aName);
		SaveClass(ClassView(createdClass), aSavePath);
		return ClassView(createdClass);
	}

	ClassView CreateClassWithoutTarget(const std::string_view aName, const std::string_view aSavePath)
	{
		return CreateClass(DataTypeView(GetDataTypeID<None*>()), aName, aSavePath);
	}

	void SetClassName(const ClassView aClassView, const std::string_view aName)
	{
		aClassView.GetClass().mName = aName;
	}

	ClassView FindClassByName(const std::string_view aName)
	{
		auto& classes = Global::GetFoundation().mClasses;

		for (HeapObject<Class>& c : classes)
		{
			if (ClassView(*c).GetName() == aName)
			{
				return ClassView(*c);
			}
		}

		return ClassView();
	}

	ClassInstanceView CreateClassInstance(const ClassView aClassView)
	{
		return ClassInstanceView(aClassView.GetClass().CreateClassInstance());
	}

	void DestroyClassInstance(ClassInstanceView aClassInstanceView)
	{
		aClassInstanceView.GetClassInstance().mClass->DestroyClassInstance(aClassInstanceView.GetClassInstance());
	}

	void EditClassInstanceVariableDefaultValue(ClassInstanceView aClassInstanceView)
	{
		ClassInstance& classInstance = aClassInstanceView.GetClassInstance();
		const std::vector<Variable>& variables = classInstance.mStructInstance.mStruct->mVariables;
		std::vector<VariableInstance>& variableInstances = classInstance.mStructInstance.mVariableInstances;

		assert(variables.size() == variableInstances.size());

		for (size_t i = 0; i < variableInstances.size(); ++i)
		{
			const Variable& variable = variables[i];
			VariableInstance& variableInstance = variableInstances[i];
			Global::GetDataTypeManager().EditData(variable.mDataTypeID, variableInstance.mDefaultValueDataPtr);
		}

	}

	NodeView CreateNode(NodeGraphView aNodeGraphView, const NodeTypeView aNodeTypeView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphView.GetVariant(), aNodeTypeView.GetID(), aPosition, aCommandTracker);
		return NodeView(nodeID, aNodeGraphView);
	}

	NodeView CreateNode(NodeGraphView aNodeGraphView, std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphView.GetVariant(), aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeView(nodeID, aNodeGraphView);
	}

	NodeView CreateNodeAutoLink(NodeGraphView aNodeGraphView, const NodeTypeView aNodeTypeView, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Node + Auto Link");
		}

		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();

		const NodeID createdNodeID = Internal::CreateNode(aNodeGraphView.GetVariant(), aNodeTypeView.GetID(), aPosition, aCommandTracker);

		const Pin& createdFromPin = nodeGraph.mPins.at(aConnection);
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(createdFromPin.mTypeID);
		const Node& createdNode = nodeGraph.mNodes.at(createdNodeID);

		const std::vector<PinID>& pinIDs = SelectByFlowType(pinType.mFlowType, createdNode.mOutputPins, createdNode.mInputPins);

		for (const PinID pinID : pinIDs)
		{
			if (Internal::TryCreateLink(nodeGraph, pinID, aConnection, aCommandTracker) != InvalidID<LinkID>())
			{
				break;
			}
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(createdNodeID, aNodeGraphView);
	}

	NodeView CreateGetterNode(NodeGraphView aNodeGraphView, const VariableView aVariableView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Getter Node");
		}

		const Variable& variable = aVariableView.GetClass().mStruct.mVariables.at(aVariableView.GetID());

		const NodeID nodeID = Internal::CreateGetterNode(aNodeGraphView.GetNodeGraph(), variable.mDataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, aNodeGraphView.GetNodeGraph(), aCommandTracker);
		Internal::BindVariable(aVariableView.GetClass(), CreateContextualNodeRef(nodeID, aNodeGraphView.GetNodeGraph()), aVariableView.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(nodeID, aNodeGraphView);
	}

	NodeView CreateSetterNode(NodeGraphView aNodeGraphView, const VariableView aVariableView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Setter Node");
		}

		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();

		const Variable& variable = aVariableView.GetClass().mStruct.mVariables.at(aVariableView.GetID());

		const NodeID nodeID = Internal::CreateSetterNode(nodeGraph, variable.mDataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, nodeGraph, aCommandTracker);
		Internal::BindVariable(aVariableView.GetClass(), CreateContextualNodeRef(nodeID, nodeGraph), aVariableView.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(nodeID, aNodeGraphView);
	}

	LinkView TryCreateLink(const PinView aPinView1, const PinView aPinView2, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		const LinkID createdLinkID = Internal::TryCreateLink(aNodeGraphView.GetNodeGraph(), aPinView1.GetID(), aPinView2.GetID(), aCommandTracker);
		return LinkView(createdLinkID, aNodeGraphView);
	}

	void DestroyLink(const LinkView aLinkView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		Internal::DestroyLink(aNodeGraphView.GetNodeGraph(), aLinkView.GetID(), aCommandTracker);
	}

	void DestroyLinksByPin(const PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinksByPin(aNodeGraphView.GetNodeGraph(), aPinView.GetID(), aCommandTracker);
	}

	void DestroyLinksByNode(NodeView aNodeView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		std::vector<LinkID> linkIDs = ScriptLinker::GetLinkIDsByNode(aNodeGraphView.GetNodeGraph(), aNodeView.GetID());

		DestroySelection({}, linkIDs, aNodeGraphView, aCommandTracker);
	}

	void DestroyNode(const NodeView aNodeView, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		return Internal::DestroyNode(aNodeGraphView.GetNodeGraph(), aNodeView.GetID(), aCommandTracker);
	}

	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Selection");
		}

		for (const LinkID linkID : aLinkIDs)
		{
			DestroyLink(LinkView(linkID, aNodeGraphView), aNodeGraphView, aCommandTracker);
		}

		for (const NodeID nodeID : aNodeIDs)
		{
			DestroyNode(NodeView(nodeID, aNodeGraphView), aNodeGraphView, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNodes(aNodeRefs, aCommandTracker);
	}

	void SetNodePosition(const NodeView aNodeView, Vec2 aPosition, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		const Vec2 oldPos = Internal::GetNode(aNodeView.GetID(), aNodeGraphView.GetNodeGraph()).mPosition;
		SetNodePosition(aNodeView, aPosition, oldPos, aNodeGraphView, aCommandTracker);
	}

	void SetNodePosition(const NodeView aNodeView, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		Internal::SetNodePosition(aNodeView.GetID(), aPosition, aOldPosition, aNodeGraphView.GetNodeGraph(), aCommandTracker);
	}

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		if (aDragData.empty())
		{
			return;
		}
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Drag nodes");
		}

		for (const auto& [mNodeID, dragData] : aDragData)
		{
			Internal::SetNodePosition(mNodeID, dragData.mEndPos, dragData.mStartPos, aNodeGraphView.GetNodeGraph(), aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	VariableView CreateVariable(const ClassView aClassView, const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(aClassView.GetClass(), aDataTypeView.GetID(), aCommandTracker);
		return VariableView(varID, aClassView);
	}

	void DestroyVariable(const VariableView aVariableView, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
		}

		struct DestroyVariableData
		{
			VarID mVarID = InvalidID<VarID>();
			Class* mClass = nullptr;
		} data;

		data.mVarID = aVariableView.GetID();
		data.mClass = &aVariableView.GetClass();

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				bool mIsDestroyed = aCommandType == eCommandType::Do;
				data.mClass->mStruct.mVariables.at(data.mVarID).mIsDestroyed = mIsDestroyed;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Destroy Variable"));
		}

		Internal::DestroyNodes(Internal::GetNodeRefsByVariableRef(VariableRef(aVariableView.GetID(), aVariableView.GetClass())), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void EditVariableDefaultValue(const VariableView aVariableView, CommandTracker*)
	{
		assert(aVariableView);
		Variable& variable = aVariableView.GetClass().mStruct.mVariables.at(aVariableView.GetID());

		if (Global::GetDataTypeManager().EditData(variable.mDataTypeID, variable.mDefaultValueDataPtr))
		{

		}
	}

	void SetVariableDataType(const VariableView aVariableView, const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(aVariableView.GetClass(), aVariableView.GetID(), aDataTypeView.GetID(), aCommandTracker);
	}

	void SetVariableName(const VariableView aVariableView, const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Variable& variable = aVariableView.GetClass().mStruct.mVariables.at(aVariableView.GetID());

		variable.mName = aName;
	}

	void DestroyVariableNodes(const VariableView aVariableView, ClassView aClassView, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNodes(Internal::GetNodeRefsByVariableRef(VariableRef(aVariableView.GetID(), aClassView.GetClass())), aCommandTracker);
	}

	void EditPin(const PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();
		Pin& pin = nodeGraph.mPins.at(aPinView.GetID());
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

		DataTypeManager& dataTypeManager = Global::GetDataTypeManager();

		const void* const copyDataPtr = [aCommandTracker, &dataTypeManager, &pinType, &pin]() -> const void*
			{
				return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.mDataTypeID, Global::Internal::GetFrameMemoryArena(), pin.mDataPtr) : nullptr;
			}();

		const bool wasEdited = dataTypeManager.EditData(pinType.mDataTypeID, pin.mDataPtr);

		if (!wasEdited || !aCommandTracker)
		{
			return;
		}


		void* const previousDataPtr = Global::GetDataTypeManager().AllocateData(pinType.mDataTypeID, Global::Internal::GetEditMemoryArena(), copyDataPtr);

		struct EditPinData
		{
			PinID mPinID = InvalidID<PinID>();
			void* mPreviousDataPtr = nullptr;
			NodeGraph* mNodeGraph = nullptr;
		} data;

		data.mPinID = aPinView.GetID();
		data.mPreviousDataPtr = previousDataPtr;
		data.mNodeGraph = &nodeGraph;

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				if (aCommandType == eCommandType::Do)
				{
					Pin& pin = data.mNodeGraph->mPins.at(data.mPinID);
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.mPreviousDataPtr);
				}
				else
				{
					Pin& pin = data.mNodeGraph->mPins.at(data.mPinID);
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.mPreviousDataPtr);
				}
			};

		aCommandTracker->RegisterCommand(Command(commandFunction, "Edit Pin"));
	}

	void SplitPin(const PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		aCommandTracker;
		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();
		const Pin& pin = nodeGraph.mPins.at(aPinView.GetID());

		if (!pin.mConnectedPinIDs.empty())
		{
			return;
		}

		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

		const DataType* pinDataType = Global::GetDataTypeManager().Find(pinType.mDataTypeID);

		if (pinDataType == nullptr)
		{
			return;
		}
	}

	bool HasPinAnyConnectedLinks(const PinView aPinView)
	{
		return !aPinView.GetConnectedPinIDs().empty();
	}

	bool HasNodeAnyConnectedLinks(const NodeView aNodeView, const NodeGraphView aNodeGraphView)
	{
		const Node& node = aNodeGraphView.GetNodeGraph().mNodes.at(aNodeView.GetID());

		auto hasConnectedLink = [](const std::vector<PinID>& aPinIDs, const NodeGraph& aNodeGraph) -> bool
			{
				for (const PinID pinID : aPinIDs)
				{
					const Pin& pin = aNodeGraph.mPins.at(pinID);
					if (!pin.mConnectedPinIDs.empty())
					{
						return true;
					}
				}

				return false;
			};

		return hasConnectedLink(node.mInputPins, aNodeGraphView.GetNodeGraph()) || hasConnectedLink(node.mOutputPins, aNodeGraphView.GetNodeGraph());
	}

	bool IsNodeReplacable(const NodeView aNodeView, const NodeGraphView aNodeGraphView)
	{
		const Node& node = Internal::GetNode(aNodeView.GetID(), aNodeGraphView.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);

		return nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None;
	}

	bool IsPinReplacable(const PinView aPinView, const NodeGraphView aNodeGraphView)
	{
		return IsNodeReplacable(NodeView(aPinView.GetNodeID(), aNodeGraphView), aNodeGraphView);
	}

	void ReplaceTemplateNode(const PinView aReplacedPinView, NodeGraphView aNodeGraphView, const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(aNodeGraphView.GetNodeGraph(), aReplacedPinView.GetNodeID(), aDataTypeView.GetID(), aCommandTracker);
	}

	void ReplaceTemplateNode(const NodeView aNodeView, NodeGraphView aNodeGraphView, const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(aNodeGraphView.GetNodeGraph(), aNodeView.GetID(), aDataTypeView.GetID(), aCommandTracker);
	}

	std::vector<DataTypeView> GetReplacableDataTypes(const PinView aPinView, const NodeGraphView aNodeGraphView)
	{
		assert(aPinView && "View is not valid");
		assert(aNodeGraphView && "View is not valid");

		return GetReplacableDataTypes(NodeView(aPinView.GetNodeID(), aNodeGraphView), aNodeGraphView);
	}

	std::vector<DataTypeView> GetReplacableDataTypes(const NodeView aNodeView, const NodeGraphView aNodeGraphView)
	{
		assert(aNodeView && "View is not valid");
		assert(aNodeGraphView && "View is not valid");

		const Node& node = Internal::GetNode(aNodeView.GetID(), aNodeGraphView.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);
		std::vector<DataTypeView> dataTypeViews;

		auto& dataTypes = Global::GetNodeTypeManager().GetTemplateMapByOperator(nodeType.mNodeRecipe.mOperatorTrait);
		dataTypeViews.reserve(dataTypes.size());
		for (auto& [dataTypeID, nodeTypeID] : dataTypes)
		{
			dataTypeViews.push_back(DataTypeView(dataTypeID));
		}

		return dataTypeViews;
	}

	void SetPinTypeName(const PinTypeView aPinTypeView, std::string_view aName)
	{
		PinType& pinType = Global::GetPinTypeManager().GetPinType(aPinTypeView.GetID());

		pinType.mName = aName;
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraphView aCopiedFromNodeGraphView)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphView.GetNodeGraph();
		EventGraph& eventGraphCopy = Global::Internal::GetNodeGraphCopy();
		eventGraphCopy = EventGraph{};

		NodeGraph& nodeGraphCopy = eventGraphCopy.mNodeGraph;

		std::vector<NodeID> createdNodeIDs;
		std::unordered_map<NodeID, NodeID> nodeConverter;
		createdNodeIDs.reserve(aNodeIDs.size());

		Vec2 avgPos;
		for (const NodeID nodeID : aNodeIDs)
		{
			const Node& node = copiedFromNodeGraph.mNodes.at(nodeID);

			const NodeID createdNodeID = Internal::CreateNode(&eventGraphCopy, node.mTypeID, node.mPosition, nullptr);
			avgPos += node.mPosition;

			createdNodeIDs.push_back(createdNodeID);
			nodeConverter.emplace(nodeID, createdNodeID);
		}

		avgPos /= static_cast<float>(aNodeIDs.size());

		for (Node& node : nodeGraphCopy.mNodes)
		{
			node.mPosition = node.mPosition - avgPos;
		}

		// Create links
		for (NodeID copiedNodeID : aNodeIDs)
		{
			const Node& copiedNode = copiedFromNodeGraph.mNodes.at(copiedNodeID);
			for (const PinID copiedInputPinID : copiedNode.mInputPins)
			{
				const Pin& copiedInputPin = Internal::GetPin(copiedInputPinID, copiedFromNodeGraph);
				const PinType& copiedInputPinType = Global::GetPinTypeManager().GetPinType(copiedInputPin.mTypeID);

				const NodeID createdNodeID = nodeConverter.at(copiedNodeID);
				const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, copiedInputPinID, nodeGraphCopy, createdNodeID);
				assert(createdInputPinID != InvalidID<PinID>());

				const std::vector<LinkID> connectedLinks = ScriptLinker::GetLinkIDsByPin(copiedFromNodeGraph, copiedInputPinID);

				for (const LinkID connectedLinkID : connectedLinks)
				{
					const Link& connectedLink = copiedFromNodeGraph.mLinks[connectedLinkID];

					const PinID connectedOutputPinID = connectedLink.mOutputPinID;
					const Pin& connectedOutputPin = Internal::GetPin(connectedOutputPinID, copiedFromNodeGraph);

					auto it = nodeConverter.find(connectedOutputPin.mNodeID);
					if (it == nodeConverter.end())
					{
						continue;
					}
					const NodeID newConnectedNodeID = it->second;
					const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, connectedOutputPinID, nodeGraphCopy, newConnectedNodeID);

					Internal::TryCreateLink(nodeGraphCopy, createdInputPinID, createdOutputPinID, nullptr);
				}

				Pin& createdInputPin = Internal::GetPin(createdInputPinID, nodeGraphCopy);

				Global::GetDataTypeManager().CopyData(copiedInputPinType.mDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}
	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraphView aTargetNodeGraphView, CommandTracker* const aCommandTracker)
	{
		const NodeGraph& nodeGraphCopy = Global::Internal::GetNodeGraphCopy().mNodeGraph;

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphView.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID = 0; sourceNodeID < nodeGraphCopy.mNodes.size(); sourceNodeID++)
		{
			const Node& node = nodeGraphCopy.mNodes.at(sourceNodeID);
			const NodeID createdNodeID = Internal::CreateNode(aTargetNodeGraphView.GetVariant(), node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Global::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

				const PinID sourcePinID = ScriptLinker::GetOpposingPinID(targetNodeGraph, createdInputPinID, nodeGraphCopy, sourceNodeID);
				const Pin& sourcePin = nodeGraphCopy.mPins.at(sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
			}
		}

		for (const Link& link : nodeGraphCopy.mLinks)
		{
			const Pin& inputPin = nodeGraphCopy.mPins[link.mInputPinID];
			const Pin& outputPin = nodeGraphCopy.mPins[link.mOutputPinID];
			const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(nodeGraphCopy, link.mInputPinID, targetNodeGraph, nodeConverter.at(inputPin.mNodeID));
			const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(nodeGraphCopy, link.mOutputPinID, targetNodeGraph, nodeConverter.at(outputPin.mNodeID));
			const LinkID createdLinkID = Internal::TryCreateLink(targetNodeGraph, createdInputPinID, createdOutputPinID, aCommandTracker);

			assert(createdLinkID != InvalidID<LinkID>());
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	CustomEventView CreateCustomEvent(const std::string_view aName)
	{
		return CustomEventView(Internal::CreateCustomEvent(aName));
	}

	void AddPinToCustomEvent(const CustomEventView aCustomEventView, const DataTypeView aDataTypeView, std::string_view aPinName, CommandTracker* const aCommandTracker)
	{
		Internal::AddPinToCustomEvent(aCustomEventView.GetID(), aDataTypeView.GetID(), aPinName, aCommandTracker);
	}

	void SetPinDataTypeAtIndexCustomEvent(const CustomEventView aCustomEventView, const DataTypeView aDataTypeView, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexCustomEvent(aCustomEventView.GetID(), aDataTypeView.GetID(), aIndex, aCommandTracker);
	}

	void SetPinNameAtIndexCustomEvent(const CustomEventView aCustomEventView, const std::string_view aName, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinNameAtIndexCustomEvent(aCustomEventView.GetID(), aName, aIndex, aCommandTracker);
	}

	void DeletePinAtIndexCustomEvent(const CustomEventView aCustomEventView, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::DeletePinAtIndexCustomEvent(aCustomEventView.GetID(), aIndex, aCommandTracker);
	}

	void SetCustomEventName(const CustomEventView aCustomEventView, std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventView.GetID());
		NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		NodeType& callerNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

		const std::string nameDirectory = Global::GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
		executorNodeType.mNodeRecipe.mName = nameDirectory + std::string(aName);
		callerNodeType.mNodeRecipe.mName = nameDirectory + "Call " + std::string(aName);
	}

	FunctionView CreateGlobalFunction(const std::string_view aName)
	{
		return FunctionView(Internal::CreateFunction(aName));
	}

	FunctionView CreateMemberFunction(const std::string_view aName, ClassView aClassView)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		aClassView.GetClass().BindFunction(id);

		return FunctionView(id);
	}

	void AddPinToFunction(const FunctionView aFunctionView, const DataTypeView aDataTypeView, const eFlowType aFlowType, std::string_view aPinName, CommandTracker* const aCommandTracker)
	{
		Internal::AddPinToFunction(aFunctionView.GetID(), aDataTypeView.GetID(), aFlowType, aPinName, aCommandTracker);
	}

	void SetPinDataTypeAtIndexFunction(const FunctionView aFunctionView, const DataTypeView aDataTypeView, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexFunction(aFunctionView.GetID(), aDataTypeView.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	void SetPinNameAtIndexFunction(const FunctionView aFunctionView, const std::string_view aName, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinNameAtIndexFunction(aFunctionView.GetID(), aName, aIndex, aFlowType, aCommandTracker);
	}

	void DeletePinAtIndexFunction(const FunctionView aFunctionView, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::DeletePinAtIndexFunction(aFunctionView.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	void SetFunctionName(const FunctionView aFunctionView, const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionView.GetID());

		function.mName = aName;
	}

	void BeginFrame()
	{
		Global::Internal::GetFrameMemoryArena().Clear();
	}

	std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		return Internal::GetInputPins(aNodeGraph, aIncludeDestroyed);
	}

	std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		return Internal::GetOutputPins(aNodeGraph, aIncludeDestroyed);
	}

	template<Predicate<const Pin&> Predicate>
	std::vector<PinView> GetPinViewsFiltered(Predicate&& aPredicate, const NodeGraphView& aNodeGraphView)
	{
		std::vector<PinView> pinViews;
		const NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();
		pinViews.reserve(nodeGraph.mPins.size());

		for (PinID pinID = 0; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinViews.push_back(PinView(pinID, aNodeGraphView));
			}
		}

		return pinViews;
	}



	std::vector<PinView> GetNonConnectedInputPinViews(const NodeGraphView aNodeGraphView)
	{
		return GetPinViewsFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Input;
			},
			aNodeGraphView
		);
	}

	std::vector<PinView> GetNonConnectedOutputPinViews(const NodeGraphView aNodeGraphView)
	{
		return GetPinViewsFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Output;
			},
			aNodeGraphView
		);
	}

	std::vector<PinView> GetNonConnectedPinViewsByFlowType(const NodeGraphView aNodeGraphView, const eFlowType aFlowType)
	{
		switch (aFlowType)
		{
		case eFlowType::Input:
			return GetNonConnectedInputPinViews(aNodeGraphView);
			break;
		case eFlowType::Output:
			return GetNonConnectedOutputPinViews(aNodeGraphView);
			break;
		default:
			break;
		}
		return std::vector<PinView>();
	}

	std::vector<PinView> GetNonConnectedPinViewsByFlowTypeAndDataType(const NodeGraphView aNodeGraphView, const eFlowType aFlowType, const DataTypeView aDataTypeView)
	{
		return GetPinViewsFiltered(
			[aFlowType, dataTypeID = aDataTypeView.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && pinType.mDataTypeID == dataTypeID;
			},
			aNodeGraphView
		);
	}

	VariableView GetVariableByNode(const NodeView aNodeView, NodeGraphView aNodeGraphView)
	{
		const VariableRef variableRef = Internal::GetVariableRefByNodeRef(GlobalNodeRef(aNodeView.GetID(), aNodeGraphView.GetNodeGraph()));
		return VariableView(variableRef.GetVarID(), ClassView(variableRef.GetClass()));
	}

	std::vector<VariableView> GetVariables(const ClassView aClassView, const bool aIncludeDestroyed)
	{
		const std::vector<Variable>& variables = aClassView.GetClass().mStruct.mVariables;
		std::vector<VariableView> views;

		views.reserve(variables.size());

		for (VarID varID = 0; varID < variables.size(); ++varID)
		{
			if (!aIncludeDestroyed)
			{
				const Variable& variable = variables.at(varID);
				if (variable.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(VariableView(varID, aClassView));
		}
		return views;
	}

	std::vector<NodeView> GetNodes(const NodeGraphView aNodeGraphView, bool aIncludeDestroyed)
	{
		const std::vector<Node>& nodes = aNodeGraphView.GetNodeGraph().mNodes;
		std::vector<NodeView> views;

		views.reserve(nodes.size());

		for (NodeID mNodeID = 0; mNodeID < nodes.size(); ++mNodeID)
		{
			if (!aIncludeDestroyed)
			{
				const Node& node = nodes.at(mNodeID);
				if (node.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(NodeView(mNodeID, aNodeGraphView));
		}
		return views;
	}

	std::vector<LinkView> GetLinks(const NodeGraphView aNodeGraphView, bool aIncludeDestroyed)
	{
		const std::vector<Link>& links = aNodeGraphView.GetNodeGraph().mLinks;
		std::vector<LinkView> views;

		views.reserve(links.size());

		for (LinkID linkID = 0; linkID < links.size(); ++linkID)
		{
			if (!aIncludeDestroyed)
			{
				const Link& link = aNodeGraphView.GetNodeGraph().mLinks.at(linkID);
				if (link.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(LinkView(linkID, aNodeGraphView));
		}
		return views;
	}

	std::vector<DataTypeView> GetDataTypes()
	{
		const auto& dataTypes = Global::GetDataTypeManager().GetDataTypes();
		std::vector<DataTypeView> views;
		views.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			views.push_back(DataTypeView(dataTypeID));
		}

		return views;
	}

	std::vector<NodeTypeView> GetNodeTypes()
	{
		const auto& nodeTypes = Global::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeView> views;
		views.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID = 0; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			views.push_back(NodeTypeView(nodeTypeID));
		}

		return views;
	}

	std::vector<FunctionView> GetFunctions()
	{
		const auto& mFunctions = Global::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionView> views;
		views.reserve(mFunctions.size());

		for (FunctionID functionID = 0; functionID < mFunctions.size(); ++functionID)
		{
			views.push_back(FunctionView(functionID));
		}

		return views;
	}

	std::vector<CustomEventView> GetCustomEvents()
	{
		const auto& customEvents = Global::GetNodeTypeManager().GetCustomEvents();
		std::vector<CustomEventView> customEventViews;
		customEventViews.reserve(customEvents.size());

		for (CustomEventID customEventID = 0; customEventID < customEvents.size(); ++customEventID)
		{
			customEventViews.push_back(CustomEventView(customEventID));
		}

		return customEventViews;
	}

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		return Global::GetNodeTypeManager().GetNodeTypesFiltered([aDataTypeID, aFlowType](const NodeType& aNodeType) -> bool
			{
				const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, aNodeType.mNodeRecipe.mInputPinTypeIDs, aNodeType.mNodeRecipe.mOutputPinTypeIDs);
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pinTypeID);
					if (pinType.mDataTypeID == aDataTypeID)
					{
						return true;
					}
				}
				return false;
			}
		);
	}

	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return Global::GetNodeTypeManager().GetNodeTypesFiltered([aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				return aBitOperation(aNodeTrait, aNodeType.mNodeRecipe.mTraits);
			}
		);
	}

	std::unordered_map<DataTypeView, std::vector<ClassView>> GetClasses()
	{
		auto& classes = Foundation::GetInstance().mClasses;

		std::unordered_map<DataTypeView, std::vector<ClassView>> views;

		for (auto& flyClass : classes)
		{
			views[DataTypeView(flyClass->mTargetID)].push_back(ClassView(*flyClass));
		}

		return views;
	}

	std::vector<ClassView> GetClassesByDataType(DataTypeView aDataTypeView)
	{
		auto& classes = Foundation::GetInstance().mClasses;

		std::vector<ClassView> views;

		for (auto& flyClass : classes)
		{
			if (flyClass->mTargetID == aDataTypeView.GetID())
			{
				views.push_back(ClassView(*flyClass));
			}
		}

		return views;
	}
}

