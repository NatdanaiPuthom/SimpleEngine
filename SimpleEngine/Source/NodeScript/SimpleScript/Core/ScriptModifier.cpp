#include "ScriptModifier.hpp"
#include "FlyClass.hpp"
#include "Node/NodeTypeManager.hpp"
#include "Pin/PinTypeManager.hpp"
#include "Node/NodeTypeRegistry.hpp"
#include "ScriptInternalModifier.hpp"
#include "ScriptCopyBuffer.hpp"
#include "Utilities/ScriptLinker.hpp"
#include "Command/ScriptCommandTracker.hpp"
#include "ScriptFoundation.hpp"
#include "Serialization/ScriptLoader.hpp"

namespace SCR
{

	namespace Internal
	{

		static void BindNodeToEvent(const NodeID aNodeID, EventGraph& anEventGraph, CommandTracker* const aCommandTracker)
		{
			struct BindData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				EventGraph* eventGraph;
			} data;

			data.mNodeID = aNodeID;
			data.eventGraph = &anEventGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					void (EventGraph:: * func)(NodeID) = aCommandType == eCommandType::Do ? &EventGraph::BindNodeToEvent : &EventGraph::UnbindNodeFromEvent;

					(data.eventGraph->*func)(data.mNodeID);
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Bind Node To Event"));
			}
		}

		static NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Node Pos");
			}

			const NodeID mNodeID = Internal::CreateNode(aNodeGraph, aNodeTypeID, aCommandTracker);

			SetNodePosition(mNodeID, aPosition, aNodeGraph, aCommandTracker);

			if (EventGraph* eventGraph = dynamic_cast<EventGraph*>(&aNodeGraph))
			{
				Internal::BindNodeToEvent(mNodeID, *eventGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return mNodeID;
		}

	}

	void SaveClass(const Class& aClass, std::string_view aSavePath)
	{
		ScriptLoader::SaveClass(aClass, aSavePath);
	}

	Class& CreateClass(const DataTypeID aTarget, const std::string& aName)
	{
		return Global::GetFoundation().CreateClass(aTarget, aName);
	}



	NodeView CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		const NodeID mNodeID = Internal::CreateNode(aNodeGraph, aNodeTypeID, aPosition, aCommandTracker);
		return NodeView(mNodeID, aNodeGraph);
	}

	NodeView CreateNodeAutoLink(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Node + Auto Link");
		}

		const NodeID mNodeID = Internal::CreateNode(aNodeGraph, aNodeTypeID, aPosition, aCommandTracker);

		const Pin& createdFromPin = ScriptProxy::GetPin(aNodeGraph, aConnection);
		const Node& createdNode = ScriptProxy::GetNode(aNodeGraph, mNodeID);

		const std::vector<PinID>& pinIDs = Global::GetPinTypeManager().GetPinType(createdFromPin.mTypeID).mFlowType == eFlowType::Input ? createdNode.mOutputPins : createdNode.mInputPins;

		for (const PinID pinID : pinIDs)
		{
			if (TryCreateLink(pinID, aConnection, aNodeGraph, aCommandTracker) != InvalidID<LinkID>())
			{
				break;
			}
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(mNodeID, aNodeGraph);
	}

	NodeView CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Node + Set Node Pos");
		}

		const NodeID mNodeID = Internal::CreateNode(aNodeGraph, aName, aSuccess, aCreateIfNameNotFound, aCommandTracker);

		SetNodePosition(mNodeID, aPosition, aNodeGraph, aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(mNodeID, aNodeGraph);
	}

	NodeView CreateGetterNode(Class& aScript, NodeGraph& aNodeGraph, const VarID aVarID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Getter Node");
		}

		const Variable& variable = aScript.GetVariableManager().mVariables.at(aVarID);
		
		const NodeID mNodeID = Internal::CreateGetterNode(aNodeGraph, variable.dataTypeID, aCommandTracker);
		SetNodePosition(mNodeID, aPosition, aNodeGraph, aCommandTracker);
		Internal::BindVariable(aScript, NodeRef{ .mNodeID = mNodeID, .mNodeGraph = &aNodeGraph }, aVarID, aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(mNodeID, aNodeGraph);
	}

	NodeView CreateSetterNode(Class& aScript, NodeGraph& aNodeGraph, const VarID aVarID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Setter Node");
		}

		const Variable& variable = aScript.GetVariableManager().mVariables.at(aVarID);

		const NodeID mNodeID = Internal::CreateSetterNode(aNodeGraph, variable.dataTypeID, aCommandTracker);
		SetNodePosition(mNodeID, aPosition, aNodeGraph, aCommandTracker);
		Internal::BindVariable(aScript, NodeRef{ .mNodeID = mNodeID, .mNodeGraph = &aNodeGraph }, aVarID, aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(mNodeID, aNodeGraph);
	}

	LinkID TryCreateLink(PinID aPinID1, PinID aPinID2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		const Link createdLink = ScriptLinker::ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
		if (!createdLink)
		{
			// Check if we can replace node with overloaded operator node
			const Pin& pin1 = ScriptProxy::GetPin(aNodeGraph, aPinID1);
			const Pin& pin2 = ScriptProxy::GetPin(aNodeGraph, aPinID2);

			const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.mTypeID);
			const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.mTypeID);

			if (pinType1.mDataTypeID == typeid(Wildcard).hash_code())
			{
				Internal::ReplaceOperatorNode(aNodeGraph, aPinID1, aPinID2, aCommandTracker);

			}
			else if (pinType2.mDataTypeID == typeid(Wildcard).hash_code())
			{
				Internal::ReplaceOperatorNode(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
			}

			return InvalidID<LinkID>();
		}

		return Internal::CreateLink(aNodeGraph, createdLink.mInputPinID, createdLink.mOutputPinID, aCommandTracker);
	}

	void DestroyLink(const LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		Internal::DestroyLink(aNodeGraph, aLinkID, aCommandTracker);
	}

	void DestoryLinksByOutputPinID(const PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		Pin& outputPin = aNodeGraph.mPinManager->myPins[aOutputPinID];

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destory Links by Output Pin");
		}
		for (PinID connectedInputPin : outputPin.mConnectedPinIDs)
		{
			DestroyLink(Link{ connectedInputPin, aOutputPinID }, aNodeGraph, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroyNode(const NodeID aNodeID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
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

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				Node& node = data.mNodeGraph->mNodeManager->mNodes[data.mNodeID];
				node.mIsDestroyed = aCommandType == eCommandType::Do;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Destroy Node"));
		}

		for (LinkID linkID : ScriptLinker::GetLinkIDsByNode(aNodeGraph, aNodeID))
		{
			DestroyLink(linkID, aNodeGraph, aCommandTracker);
		}


		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Selection");
		}

		for (const LinkID linkID : aLinkIDs)
		{
			DestroyLink(linkID, aNodeGraph, aCommandTracker);
		}

		for (const NodeID mNodeID : aNodeIDs)
		{
			DestroyNode(mNodeID, aNodeGraph, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Nodes");
		}

		for (const NodeRef& nodeRef : aNodeRefs)
		{
			DestroyNode(nodeRef.mNodeID, *nodeRef.mNodeGraph, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void SetNodePosition(const NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		SetNodePosition(aNodeID, aPosition, aNodeGraph.mNodeManager->mNodes.at(aNodeID).mPosition, aNodeGraph, aCommandTracker);
	}

	void SetNodePosition(const NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{

		struct SetNodePositionData
		{
			NodeID mNodeID = InvalidID<NodeID>();
			Vec2 oldPos;
			Vec2 newPos;
			NodeGraph* mNodeGraph = nullptr;
		} data;

		if (aPosition == aOldPosition)
		{
			return;
		}


		data.mNodeID = aNodeID;
		data.oldPos = aOldPosition;
		data.newPos = aPosition;
		data.mNodeGraph = &aNodeGraph;

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				Node& node = data.mNodeGraph->mNodeManager->mNodes.at(data.mNodeID);
				const Vec2& pos = aCommandType == eCommandType::Do ? data.newPos : data.oldPos;
				node.mPosition = pos;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Set Node Position"));
		}


	}

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
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
			SetNodePosition(mNodeID, dragData.endPos, dragData.startPos, aNodeGraph, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	VariableView CreateVariable(Class& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(aScript, aDataTypeID, aCommandTracker);
		return VariableView(varID, aScript);
	}

	void DestroyVariable(VarID aVarID, Class& aScript, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
		}

		struct DestroyVariableData
		{
			VarID varID = InvalidID<VarID>();
			Class* script = nullptr;
		} data;

		data.varID = aVarID;
		data.script = &aScript;

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				bool mIsDestroyed = aCommandType == eCommandType::Do;
				ScriptProxy::GetVariableRef(*data.script, data.varID).mIsDestroyed = mIsDestroyed;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Destroy Variable"));
		}

		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aScript);
		DestroyNodes(variableManager.GetNodeRefsByVarID(aVarID), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

	}

	void EditPin(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		const Pin& pin = aNodeGraph.mPinManager->myPins[aPinID];
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


		void* previousDataPtr = Global::GetDataTypeManager().AllocateData(pinType.mDataTypeID, Global::Internal::GetEditMemoryArena(), copyDataPtr);

		struct EditPinData
		{
			PinID pinID = InvalidID<PinID>();
			void* previousDataPtr = nullptr;
			NodeGraph* mNodeGraph = nullptr;
		} data;

		data.pinID = aPinID;
		data.previousDataPtr = previousDataPtr;
		data.mNodeGraph = &aNodeGraph;

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				if (aCommandType == eCommandType::Do)
				{
					const Pin& pin = data.mNodeGraph->mPinManager->myPins[data.pinID];
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.previousDataPtr);
				}
				else
				{
					const Pin& pin = data.mNodeGraph->mPinManager->myPins[data.pinID];
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.previousDataPtr);
				}
			};

		aCommandTracker->RegisterCommand(Command(commandFunction, "Edit Pin"));
	}

	void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
	{
		aCommandTracker;
		const Pin& pin = ScriptProxy::GetPin(aNodeGraph, aPinID);

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

		const std::vector<Property>& mProperties = pinDataType->mProperties;
		if (mProperties.empty())
		{
			return;
		}

		for (const Property& property : mProperties)
		{
			const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(property.mName, pinType.mFlowType, property.mTypeID, CreatePinSetFunction());
			Internal::CreatePin(aNodeGraph, pin.mNodeID, createdPinTypeID);
		}
	}

	void EditVariableDefaultValue(const VarID aVarID, Class& aScript, CommandTracker*)
	{
		const Variable& variable = ScriptProxy::GetVariable(aScript, aVarID);

		if (Global::GetDataTypeManager().EditData(variable.dataTypeID, variable.defaultValueDataPtr))
		{

		}
	}

	void SetVariableDataType(const VarID aVarID, const DataTypeID aDataTypeID, Class& aScript, CommandTracker* const aCommandTracker)
	{
		Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

		void* defaultValueDataPtr = Global::GetDataTypeManager().AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryArena(aScript));

		variable.dataTypeID = aDataTypeID;
		variable.defaultValueDataPtr = defaultValueDataPtr;

		DestroyVariableNodes(aVarID, aScript, aCommandTracker);
	}

	void SetVariableName(const VarID aVarID, const std::string_view aName, Class& aScript)
	{
		Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

		variable.mName = aName;
	}

	void DestroyVariableNodes(const VarID aVarID, Class& aScript, CommandTracker* const aCommandTracker)
	{
		DestroyNodes(aScript.GetVariableManager().GetNodeRefsByVarID(aVarID), aCommandTracker);
	}

	void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName)
	{
		PinType& pinType = Global::GetPinTypeManager().GetPinType(aPinTypeID);

		pinType.mName = aName;
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraph& aNodeGraph)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		CopyBuffer& copyBuffer = Global::Internal::GetCopyBuffer();
		copyBuffer = CopyBuffer{};

		std::vector<NodeID> createdNodeIDs;
		std::unordered_map<NodeID, NodeID> nodeConverter;
		createdNodeIDs.reserve(aNodeIDs.size());

		Vec2 avgPos;
		for (NodeID mNodeID : aNodeIDs)
		{
			const Node& node = ScriptProxy::GetNode(aNodeGraph, mNodeID);

			const NodeID createdNodeID = Internal::CreateNode(copyBuffer.mNodeGraph, node.mTypeID, node.mPosition, nullptr);
			avgPos += node.mPosition;

			createdNodeIDs.push_back(createdNodeID);
			nodeConverter.emplace(mNodeID, createdNodeID);
		}

		avgPos /= static_cast<float>(aNodeIDs.size());

		for (Node& node : copyBuffer.mNodeGraph.mNodeManager->mNodes)
		{
			node.mPosition = node.mPosition - avgPos;
		}

		// Create links
		for (NodeID copiedNodeID : aNodeIDs)
		{
			const Node& copiedNode = ScriptProxy::GetNode(aNodeGraph, copiedNodeID);
			for (PinID copiedInputPinID : copiedNode.mInputPins)
			{
				const Pin& copiedInputPin = ScriptProxy::GetPin(aNodeGraph, copiedInputPinID);
				const PinType& copiedInputPinType = Global::GetPinTypeManager().GetPinType(copiedInputPin.mTypeID);

				const NodeID createdNodeID = nodeConverter.at(copiedNodeID);
				const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(aNodeGraph, copiedInputPinID, copyBuffer.mNodeGraph, createdNodeID);
				assert(createdInputPinID != InvalidID<PinID>());

				const std::vector<LinkID> connectedLinks = ScriptLinker::GetLinkIDsByPin(aNodeGraph, copiedInputPinID);

				for (const LinkID connectedLinkID : connectedLinks)
				{
					const Link& connectedLink = aNodeGraph.mLinks[connectedLinkID];

					const PinID connectedOutputPinID = connectedLink.mOutputPinID;
					const Pin& connectedOutputPin = ScriptProxy::GetPin(aNodeGraph, connectedOutputPinID);

					auto it = nodeConverter.find(connectedOutputPin.mNodeID);
					if (it == nodeConverter.end())
					{
						continue;
					}
					const NodeID newConnectedNodeID = it->second;
					const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(aNodeGraph, connectedOutputPinID, copyBuffer.mNodeGraph, newConnectedNodeID);

					TryCreateLink(createdInputPinID, createdOutputPinID, copyBuffer.mNodeGraph, nullptr);
				}

				const Pin& createdInputPin = ScriptProxy::GetPin(copyBuffer.mNodeGraph, createdInputPinID);

				Global::GetDataTypeManager().CopyData(copiedInputPinType.mDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}

		// Copy Data

	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
	{
		CopyBuffer& copyBuffer = Global::Internal::GetCopyBuffer();

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID = 0; sourceNodeID < copyBuffer.mNodeGraph.mNodeManager->mNodes.size(); sourceNodeID++)
		{
			const Node& node = copyBuffer.mNodeGraph.mNodeManager->mNodes[sourceNodeID];
			const NodeID createdNodeID = Internal::CreateNode(aNodeGraph, node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = ScriptProxy::GetNode(aNodeGraph, createdNodeID);
			DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				const Pin& createdInputPin = ScriptProxy::GetPin(aNodeGraph, createdInputPinID);
				const PinType& createdInputPinType = Global::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

				const PinID sourcePinID = ScriptLinker::GetOpposingPinID(aNodeGraph, createdInputPinID, copyBuffer.mNodeGraph, sourceNodeID);
				const Pin& sourcePin = ScriptProxy::GetPin(copyBuffer.mNodeGraph, sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
			}
		}

		for (const Link& link : copyBuffer.mNodeGraph.mLinks)
		{
			const Pin& inputPin = copyBuffer.mNodeGraph.mPinManager->myPins[link.mInputPinID];
			const Pin& outputPin = copyBuffer.mNodeGraph.mPinManager->myPins[link.mOutputPinID];
			const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.mNodeGraph, link.mInputPinID, aNodeGraph, nodeConverter.at(inputPin.mNodeID));
			const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.mNodeGraph, link.mOutputPinID, aNodeGraph, nodeConverter.at(outputPin.mNodeID));
			LinkID createdLinkID = TryCreateLink(createdInputPinID, createdOutputPinID, aNodeGraph, aCommandTracker);

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

	static PinTypeID AddPinToNodeType(const DataTypeID aDataTypeID, const NodeTypeID aNodeTypeID, const eFlowType aFlowType, std::string_view aPinName)
	{
		NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

		const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(aPinName, aFlowType, aDataTypeID, CreatePinSetFunction());

		std::vector<PinTypeID>& pinTypeIDs = aFlowType == eFlowType::Input ? nodeType.mNodeRecipe.mInputPinTypeIDs : nodeType.mNodeRecipe.mOutputPinTypeIDs;
		pinTypeIDs.push_back(createdPinTypeID);

		for (const NodeRef& nodeRef : nodeType.mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			const PinID createdPinID = Internal::CreatePin(*nodeRef.mNodeGraph, nodeRef.mNodeID, createdPinTypeID);

			pinIDs.push_back(createdPinID);
		}

		return createdPinTypeID;
	}


	static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		PinTypeManager& pinTypeManager = Global::GetPinTypeManager();

		NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
		std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);
		const PinTypeID oldPinTypeID = pinTypeIDs.at(anIndex);
		const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

		const PinTypeID newPinTypeID = pinTypeManager.Create(oldPinType.mName, aFlowType, aDataTypeID, CreatePinSetFunction());

		pinTypeIDs.at(anIndex) = newPinTypeID;

		const std::vector<NodeRef>& mNodeRefs = nodeType.mNodeRefs;
		for (const NodeRef& nodeRef : mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			const PinID createdPinID = Internal::CreatePin(*nodeRef.mNodeGraph, nodeRef.mNodeID, newPinTypeID);
			pinIDs.at(anIndex) = createdPinID;
		}
	}

	static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const eFlowType aFlowType)
	{
		NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

		std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

		assert(anIndex < pinTypeIDs.size());

		pinTypeIDs.erase(pinTypeIDs.begin() + anIndex);

		for (const NodeRef& nodeRef : nodeType.mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			pinIDs.erase(pinIDs.begin() + anIndex);
		}
	}

	void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aCustomEventID, std::string_view aPinName)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		AddPinToNodeType(aDataTypeID, customEvent.GetExecutorTypeID(), eFlowType::Output, aPinName);
		AddPinToNodeType(aDataTypeID, customEvent.GetCallerTypeID(), eFlowType::Input, aPinName);
	}

	void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aCustomEventID)
	{
		if (anIndex == 0)
		{
			return;
		}
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, aDataTypeID, eFlowType::Output);
		SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, aDataTypeID, eFlowType::Input);
	}

	void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aCustomEventID)
	{
		if (anIndex == 0)
		{
			return;
		}

		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, eFlowType::Input);
		DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, eFlowType::Output);
	}

	void SetCustomEventName(const CustomEventID aCustomEventID, std::string_view aName)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);
		NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		NodeType& callerNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

		const std::string nameDirectory = Global::GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
		executorNodeType.mName = nameDirectory + std::string(aName);
		callerNodeType.mName = nameDirectory + "Call " + std::string(aName);
	}

	FunctionView CreateGlobalFunction(const std::string& aName)
	{
		return FunctionView(Internal::CreateFunction(aName));
	}

	FunctionView CreateMemberFunction(const std::string& aName, Class& aScript)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		aScript.BindFunction(id);

		return FunctionView(id);
	}

	void AddPinToFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName)
	{
		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const Function& function = nodeTypeManager.GetFunction(aFunctionID);


		AddPinToNodeType(aDataTypeID, function.GetCallerNodeTypeID(), aFlowType, aPinName);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
		AddPinToNodeType(aDataTypeID, inputOutputNodeTypeID, InvertFlowType(aFlowType), aPinName);
	}

	void SetPinAtIndexFunction(const FunctionID aFunctionID, const size_t anIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionID);

		SetPinAtIndexNodeType(function.GetCallerNodeTypeID(), anIndex, aDataTypeID, aFlowType);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
		SetPinAtIndexNodeType(inputOutputNodeTypeID, anIndex, aDataTypeID, InvertFlowType(aFlowType));
	}

	void DeletePinAtIndexFunction(const FunctionID aFunctionID, const size_t anIndex, const eFlowType aFlowType)
	{
		const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionID);

		DeletePinAtIndexNodeType(function.GetCallerNodeTypeID(), anIndex, aFlowType);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.GetInputNodeTypeID(), function.GetOutputNodeTypeID());
		DeletePinAtIndexNodeType(inputOutputNodeTypeID, anIndex, InvertFlowType(aFlowType));
	}

	void SetFunctionName(const FunctionID aFunctionID, std::string_view aName)
	{
		Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionID);

		function.SetName(aName);
	}

	void BeginFrame()
	{
		Global::Internal::GetFrameMemoryArena().Clear();
	}

	VariableView GetVariableByNodeID(const NodeID aNodeID, NodeGraph& aNodeGraph, const Class& aScript)
	{
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aScript);
		const VarID varID = variableManager.GetVariableIDByNodeRef(NodeRef{ .mNodeID = aNodeID, .mNodeGraph = &aNodeGraph });
		return VariableView(varID, aScript);
	}

	std::vector<VariableView> GetVariables(const Class& aScript, bool aIncludeDestroyed)
	{
		const std::vector<Variable>& variables = aScript.GetVariableManager().mVariables;
		std::vector<VariableView> variableViews;

		variableViews.reserve(variables.size());

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
			variableViews.push_back(VariableView(varID, aScript));
		}
		return variableViews;
	}

	std::vector<NodeView> GetNodes(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		const std::vector<Node>& nodes = aNodeGraph.mNodeManager->mNodes;
		std::vector<NodeView> nodeViews;

		nodeViews.reserve(nodes.size());

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
			nodeViews.push_back(NodeView(mNodeID, aNodeGraph));
		}
		return nodeViews;
	}

	std::vector<LinkView> GetLinks(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		const std::vector<Link>& links = aNodeGraph.mLinks;
		std::vector<LinkView> linkViews;

		linkViews.reserve(links.size());

		for (LinkID linkID = 0; linkID < links.size(); ++linkID)
		{
			if (!aIncludeDestroyed)
			{
				const Link& link = aNodeGraph.mLinks.at(linkID);
				if (link.mIsDestroyed)
				{
					continue;
				}
			}
			linkViews.push_back(LinkView(linkID, aNodeGraph));
		}
		return linkViews;
	}

	std::vector<NodeTypeView> GetNodeTypes()
	{
		const auto& nodeTypes = Global::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeView> nodeTypeViews;
		nodeTypeViews.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID = 0; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			nodeTypeViews.push_back(NodeTypeView(nodeTypeID));
		}

		return nodeTypeViews;
	}

	std::vector<FunctionView> GetFunctions()
	{
		const auto& mFunctions = Global::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionView> functionViews;
		functionViews.reserve(mFunctions.size());

		for (FunctionID functionID = 0; functionID < mFunctions.size(); ++functionID)
		{
			functionViews.push_back(FunctionView(functionID));
		}

		return functionViews;
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

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& GetScripts()
	{
		return ScriptFoundation::GetInstance().GetScripts();
	}


}

