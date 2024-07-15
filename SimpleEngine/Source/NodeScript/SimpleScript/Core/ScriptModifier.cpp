#include "ScriptModifier.h"
#include "Script.h"
#include "Node/NodeTypeManager.h"
#include "Pin/PinTypeManager.h"
#include "Node/NodeTypeRegistry.h"
#include "ScriptManager.h"
#include "ScriptInternalModifier.h"
#include "ScriptCopyBuffer.h"
#include "Command/ScriptFunctionCommand.h"
#include "Utilities/ScriptLinker.h"
#include "Command/ScriptCommandTracker.h"
#include "ScriptFoundation.h"

namespace SCR
{


	namespace Modify
	{
		namespace Internal
		{

			static void BindNodeToEvent(const NodeID aNodeID, EventGraph& anEventGraph, CommandTracker* const aCommandTracker)
			{
				struct BindData
				{
					NodeID nodeID = InvalidID<NodeID>();
					EventGraph* eventGraph;
				} data;

				data.nodeID = aNodeID;
				data.eventGraph = &anEventGraph;

				auto commandFunction = [data](eCommandType aCommandType) -> void
					{
						void (EventGraph:: * func)(NodeID) = aCommandType == eCommandType::Do ? &EventGraph::BindNodeToEvent : &EventGraph::UnbindNodeFromEvent;

						(data.eventGraph->*func)(data.nodeID);
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
		}

		Script& CreateScript(const DataTypeID aTarget, const std::string& aName)
		{
			return Global::GetFoundation().CreateScript(aTarget, aName);
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, const ScriptVec2 aPosition, CommandTracker* const aCommandTracker)
		{

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Node Pos");
			}

			NodeID nodeID = InternalModifier::CreateNode(aNodeGraph, aNodeTypeID, aCommandTracker);

			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);

			if (EventGraph* eventGraph = dynamic_cast<EventGraph*>(&aNodeGraph))
			{
				Internal::BindNodeToEvent(nodeID, *eventGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}


		NodeID CreateNodeAutoLink(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, const PinID aConnection, const ScriptVec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Auto Link");
			}

			NodeID nodeID = CreateNode(aNodeGraph, aNodeTypeID, aPosition, aCommandTracker);

			const Pin& createdFromPin = ScriptProxy::GetPin(aNodeGraph, aConnection);
			const Node& createdNode = ScriptProxy::GetNode(aNodeGraph, nodeID);

			const std::vector<PinID>& pinIDs = PinTypeManager::GetPinType(createdFromPin.typeID).flowType == eFlowType::Input ? createdNode.outputPins : createdNode.inputPins;

			for (PinID pinID : pinIDs)
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

			return nodeID;
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, const ScriptVec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Node Pos");
			}

			NodeID nodeID = InternalModifier::CreateNode(aNodeGraph, aName, aSuccess, aCreateIfNameNotFound, aCommandTracker);

			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateGetterNode(Script& aScript, NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, const VarID aVarID, const ScriptVec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Getter Node");
			}

			NodeID nodeID = InternalModifier::CreateGetterNode(aNodeGraph, aDataTypeID, aCommandTracker);
			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			InternalModifier::BindVariable(aScript, nodeID, aVarID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}

		NodeID CreateSetterNode(Script& aScript, NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, const VarID aVarID, const ScriptVec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Setter Node");
			}

			NodeID nodeID = InternalModifier::CreateSetterNode(aNodeGraph, aDataTypeID, aCommandTracker);
			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			InternalModifier::BindVariable(aScript, nodeID, aVarID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}

		LinkID TryCreateLink(PinID aPinID1, PinID aPinID2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			Link createdLink = ScriptLinker::ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded operator node
				const Pin& pin1 = ScriptProxy::GetPin(aNodeGraph, aPinID1);
				const Pin& pin2 = ScriptProxy::GetPin(aNodeGraph, aPinID2);

				const PinType& pinType1 = PinTypeManager::GetPinType(pin1.typeID);
				const PinType& pinType2 = PinTypeManager::GetPinType(pin2.typeID);

				if (pinType1.dataTypeID == typeid(Wildcard).hash_code())
				{
					InternalModifier::ReplaceOperatorNode(aNodeGraph, aPinID1, aPinID2, aCommandTracker);

				}
				else if (pinType2.dataTypeID == typeid(Wildcard).hash_code())
				{
					InternalModifier::ReplaceOperatorNode(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
				}

				return InvalidID<LinkID>();
			}

			return InternalModifier::CreateLink(aNodeGraph, createdLink.inputPinID, createdLink.outputPinID, aCommandTracker);
		}

		void DestroyLink(const LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			InternalModifier::DestroyLink(aNodeGraph, aLinkID, aCommandTracker);
		}

		void DestoryLinksByOutputPinID(const PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			Pin& outputPin = aNodeGraph.myPinManager->myPins[aOutputPinID];

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destory Links by Output Pin");
			}
			for (PinID connectedInputPin : outputPin.connectedPinIDs)
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
				NodeID nodeID = InvalidID<NodeID>();
				NodeGraph* nodeGraph = nullptr;
			} data;

			data.nodeID = aNodeID;
			data.nodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = data.nodeGraph->myNodeManager->myNodes[data.nodeID];
					node.isDestroyed = aCommandType == eCommandType::Do;
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

			for (LinkID linkID : aLinkIDs)
			{
				DestroyLink(linkID, aNodeGraph, aCommandTracker);
			}

			for (NodeID nodeID : aNodeIDs)
			{
				DestroyNode(nodeID, aNodeGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			SetNodePosition(aNodeID, aPosition, aNodeGraph.myNodeManager->myNodes.at(aNodeID).position, aNodeGraph, aCommandTracker);
		}

		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{

			struct SetNodePositionData
			{
				NodeID nodeID = InvalidID<NodeID>();
				ScriptVec2 oldPos;
				ScriptVec2 newPos;
				NodeGraph* nodeGraph = nullptr;
			} data;

			if (aPosition == aOldPosition)
			{
				return;
			}


			data.nodeID = aNodeID;
			data.oldPos = aOldPosition;
			data.newPos = aPosition;
			data.nodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = data.nodeGraph->myNodeManager->myNodes.at(data.nodeID);
					const ScriptVec2& pos = aCommandType == eCommandType::Do ? data.newPos : data.oldPos;
					node.position = pos;
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

			for (const auto& [nodeID, dragData] : aDragData)
			{
				SetNodePosition(nodeID, dragData.endPos, dragData.startPos, aNodeGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(aScript);
			VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(id, aDataTypeID, aScript, aCommandTracker);
			return id;
		}

		void DestroyVariable(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
			}

			struct DestroyVariableData
			{
				VarID varID = InvalidID<VarID>();
				Script* script = nullptr;
			} data;

			data.varID = aVarID;
			data.script = &aScript;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					bool isDestroyed = aCommandType == eCommandType::Do;
					ScriptProxy::GetVariableRef(*data.script, data.varID).isDestroyed = isDestroyed;
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
			DestroySelection(variableManager.GetNodeIDsByVarID(aVarID), {}, aScript.GetEventGraph(), aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

		}

		void EditPin(const PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			const Pin& pin = aNodeGraph.myPinManager->myPins[aPinID];
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

			DataTypeManager& dataTypeManager = Global::GetDataTypeManager();

			const void* const copyDataPtr = [aCommandTracker, &dataTypeManager, &pinType, &pin]() -> const void*
				{
					return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.dataTypeID, Global::Internal::GetFrameMemoryArena(), pin.dataPtr) : nullptr;
				}();

			const bool wasEdited = dataTypeManager.EditData(pinType.dataTypeID, pin.dataPtr);

			if (!wasEdited || !aCommandTracker)
			{
				return;
			}


			void* previousDataPtr = Global::GetDataTypeManager().AllocateData(pinType.dataTypeID, Global::Internal::GetEditMemoryArena(), copyDataPtr);

			struct EditPinData
			{
				PinID pinID = InvalidID<PinID>();
				void* previousDataPtr = nullptr;
				NodeGraph* nodeGraph = nullptr;
			} data;

			data.pinID = aPinID;
			data.previousDataPtr = previousDataPtr;
			data.nodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						const Pin& pin = data.nodeGraph->myPinManager->myPins[data.pinID];
						const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

						Global::GetDataTypeManager().SwapData(pinType.dataTypeID, pin.dataPtr, data.previousDataPtr);
					}
					else
					{
						const Pin& pin = data.nodeGraph->myPinManager->myPins[data.pinID];
						const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

						Global::GetDataTypeManager().SwapData(pinType.dataTypeID, pin.dataPtr, data.previousDataPtr);
					}
				};

			aCommandTracker->RegisterCommand(Command(commandFunction, "Edit Pin"));
		}

		void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			aCommandTracker;
			const Pin& pin = ScriptProxy::GetPin(aNodeGraph, aPinID);

			if (!pin.connectedPinIDs.empty())
			{
				return;
			}

			const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.typeID);

			const DataType* pinDataType = Global::GetDataTypeManager().Find(pinType.dataTypeID);

			if (pinDataType == nullptr)
			{
				return;
			}

			const std::vector<Property>& properties = pinDataType->properties;
			if (properties.empty())
			{
				return;
			}

			for (const Property& property : properties)
			{
				const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(property.name, pinType.flowType, property.typeID, CreatePinSetFunction());
				InternalModifier::CreatePin(aNodeGraph, pin.nodeID, createdPinTypeID);
			}
		}

		void EditVariableDefaultValue(const VarID aVarID, Script& aScript, CommandTracker*)
		{
			const Variable& variable = ScriptProxy::GetVariable(aScript, aVarID);

			if (Global::GetDataTypeManager().EditData(variable.dataTypeID, variable.defaultValueDataPtr))
			{

			}
		}

		void SetVariableDataType(const VarID aVarID, const DataTypeID aDataTypeID, Script& aScript, CommandTracker* const aCommandTracker)
		{
			Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

			void* defaultValueDataPtr = Global::GetDataTypeManager().AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryArena(aScript));

			variable.dataTypeID = aDataTypeID;
			variable.defaultValueDataPtr = defaultValueDataPtr;

			DestroyVariableNodes(aVarID, aScript, aScript.GetEventGraph(), aCommandTracker);
		}

		void SetVariableName(VarID aVarID, const std::string& aName, Script& aScript)
		{
			Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

			variable.name = aName;
		}

		void DestroyVariableNodes(const VarID aVarID, Script& aScript, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			DestroySelection(ScriptProxy::GetVariableManager(aScript).GetNodeIDsByVarID(aVarID), {}, aNodeGraph, aCommandTracker);
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

			ScriptVec2 avgPos;
			for (NodeID nodeID : aNodeIDs)
			{
				const Node& node = ScriptProxy::GetNode(aNodeGraph, nodeID);

				const NodeID createdNodeID = CreateNode(copyBuffer.nodeGraph, node.typeID, node.position, nullptr);
				avgPos += node.position;

				createdNodeIDs.push_back(createdNodeID);
				nodeConverter.emplace(nodeID, createdNodeID);
			}

			avgPos /= static_cast<float>(aNodeIDs.size());

			for (Node& node : copyBuffer.nodeGraph.myNodeManager->myNodes)
			{
				node.position = node.position - avgPos;
			}

			// Create links
			for (NodeID copiedNodeID : aNodeIDs)
			{
				const Node& copiedNode = ScriptProxy::GetNode(aNodeGraph, copiedNodeID);
				for (PinID copiedInputPinID : copiedNode.inputPins)
				{
					const Pin& copiedInputPin = ScriptProxy::GetPin(aNodeGraph, copiedInputPinID);
					const PinType& copiedInputPinType = Global::GetPinTypeManager().GetPinType(copiedInputPin.typeID);

					const NodeID createdNodeID = nodeConverter.at(copiedNodeID);
					const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(aNodeGraph, copiedInputPinID, copyBuffer.nodeGraph, createdNodeID);
					assert(createdInputPinID != InvalidID<PinID>());

					const std::vector<LinkID> connectedLinks = ScriptLinker::GetLinkIDsByPin(aNodeGraph, copiedInputPinID);

					for (const LinkID connectedLinkID : connectedLinks)
					{
						const Link& connectedLink = aNodeGraph.myLinks[connectedLinkID];

						const PinID connectedOutputPinID = connectedLink.outputPinID;
						const Pin& connectedOutputPin = ScriptProxy::GetPin(aNodeGraph, connectedOutputPinID);

						auto it = nodeConverter.find(connectedOutputPin.nodeID);
						if (it == nodeConverter.end())
						{
							continue;
						}
						const NodeID newConnectedNodeID = it->second;
						const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(aNodeGraph, connectedOutputPinID, copyBuffer.nodeGraph, newConnectedNodeID);

						TryCreateLink(createdInputPinID, createdOutputPinID, copyBuffer.nodeGraph, nullptr);
					}

					const Pin& createdInputPin = ScriptProxy::GetPin(copyBuffer.nodeGraph, createdInputPinID);

					Global::GetDataTypeManager().CopyData(copiedInputPinType.dataTypeID, createdInputPin.dataPtr, copiedInputPin.dataPtr);
				}
			}

			// Copy Data

		}

		void PasteCopyBuffer(const ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			CopyBuffer& copyBuffer = Global::Internal::GetCopyBuffer();

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Paste Nodes");
			}

			std::unordered_map<NodeID, NodeID> nodeConverter;

			for (NodeID sourceNodeID = 0; sourceNodeID < copyBuffer.nodeGraph.myNodeManager->myNodes.size(); sourceNodeID++)
			{
				const Node& node = copyBuffer.nodeGraph.myNodeManager->myNodes[sourceNodeID];
				NodeID createdNodeID = CreateNode(aNodeGraph, node.typeID, aPosition + node.position, aCommandTracker);
				nodeConverter.emplace(sourceNodeID, createdNodeID);

				const Node& createdNode = ScriptProxy::GetNode(aNodeGraph, createdNodeID);
				DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
				for (const PinID createdInputPinID : createdNode.inputPins)
				{
					const Pin& createdInputPin = ScriptProxy::GetPin(aNodeGraph, createdInputPinID);
					const PinType& createdInputPinType = Global::GetPinTypeManager().GetPinType(createdInputPin.typeID);

					const PinID sourcePinID = ScriptLinker::GetOpposingPinID(aNodeGraph, createdInputPinID, copyBuffer.nodeGraph, sourceNodeID);
					const Pin& sourcePin = ScriptProxy::GetPin(copyBuffer.nodeGraph, sourcePinID);
					dataTypeManager.CopyData(createdInputPinType.dataTypeID, createdInputPin.dataPtr, sourcePin.dataPtr);
				}
			}

			for (const Link& link : copyBuffer.nodeGraph.myLinks)
			{
				const Pin& inputPin = copyBuffer.nodeGraph.myPinManager->myPins[link.inputPinID];
				const Pin& outputPin = copyBuffer.nodeGraph.myPinManager->myPins[link.outputPinID];
				const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.nodeGraph, link.inputPinID, aNodeGraph, nodeConverter.at(inputPin.nodeID));
				const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.nodeGraph, link.outputPinID, aNodeGraph, nodeConverter.at(outputPin.nodeID));
				LinkID createdLinkID = TryCreateLink(createdInputPinID, createdOutputPinID, aNodeGraph, aCommandTracker);

				assert(createdLinkID != InvalidID<LinkID>());
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		CustomEventID CreateCustomEvent(const std::string& aName)
		{
			return InternalModifier::CreateCustomEvent(aName);
		}

		static PinTypeID AddPinToNodeType(const DataTypeID aDataTypeID, const NodeTypeID aNodeTypeID, const eFlowType aFlowType, const std::string& aPinName)
		{
			NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID);

			const PinTypeID createdPinTypeID = PinTypeManager::Create(aPinName, aFlowType, aDataTypeID, CreatePinSetFunction());

			std::vector<PinTypeID>& pinTypeIDs = aFlowType == eFlowType::Input ? nodeType.nodeRecipe.inputPinTypeIDs : nodeType.nodeRecipe.outputPinTypeIDs;
			pinTypeIDs.push_back(createdPinTypeID);

			for (const NodeRef& nodeRef : nodeType.nodeRefs)
			{
				Node& node = ScriptProxy::GetNodeRef(*nodeRef.nodeGraph, nodeRef.nodeID);

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.inputPins, node.outputPins);
				const PinID createdPinID = InternalModifier::CreatePin(*nodeRef.nodeGraph, nodeRef.nodeID, createdPinTypeID);

				pinIDs.push_back(createdPinID);
			}

			return createdPinTypeID;
		}


		static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
		{
			NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
			PinTypeManager& pinTypeManager = Global::GetPinTypeManager();

			NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.nodeRecipe.inputPinTypeIDs, nodeType.nodeRecipe.outputPinTypeIDs);
			const PinTypeID oldPinTypeID = pinTypeIDs.at(anIndex);
			const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

			const PinTypeID newPinTypeID = pinTypeManager.Create(oldPinType.name, aFlowType, aDataTypeID, CreatePinSetFunction());

			pinTypeIDs.at(anIndex) = newPinTypeID;

			const std::vector<NodeRef>& nodeRefs = nodeType.nodeRefs;
			for (const NodeRef& nodeRef : nodeRefs)
			{
				Node& node = ScriptProxy::GetNodeRef(*nodeRef.nodeGraph, nodeRef.nodeID);

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.inputPins, node.outputPins);
				const PinID createdPinID = InternalModifier::CreatePin(*nodeRef.nodeGraph, nodeRef.nodeID, newPinTypeID);
				pinIDs.at(anIndex) = createdPinID;
			}
		}

		static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const eFlowType aFlowType)
		{
			NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID);

			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.nodeRecipe.inputPinTypeIDs, nodeType.nodeRecipe.outputPinTypeIDs);

			assert(anIndex < pinTypeIDs.size());

			pinTypeIDs.erase(pinTypeIDs.begin() + anIndex);

			for (const NodeRef& nodeRef : nodeType.nodeRefs)
			{
				Node& node = ScriptProxy::GetNodeRef(*nodeRef.nodeGraph, nodeRef.nodeID);

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.inputPins, node.outputPins);
				pinIDs.erase(pinIDs.begin() + anIndex);
			}
		}

		void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aCustomEventID, const std::string& aPinName)
		{
			CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(aCustomEventID);

			AddPinToNodeType(aDataTypeID, customEvent.GetExecutorTypeID(), eFlowType::Output, aPinName);
			AddPinToNodeType(aDataTypeID, customEvent.GetCallerTypeID(), eFlowType::Input, aPinName);
		}

		void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aCustomEventID)
		{
			if (anIndex == 0)
			{
				return;
			}
			CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(aCustomEventID);

			SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, aDataTypeID, eFlowType::Output);
			SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, aDataTypeID, eFlowType::Input);
		}

		void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aCustomEventID)
		{
			if (anIndex == 0)
			{
				return;
			}

			const CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(aCustomEventID);

			DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, eFlowType::Input);
			DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, eFlowType::Output);
		}


		FunctionID CreateGlobalFunction(const std::string& aName)
		{
			return InternalModifier::CreateFunction(aName);
		}

		FunctionID CreateMemberFunction(const std::string& aName, Script& aScript)
		{
			FunctionID id = InternalModifier::CreateFunction(aName);
			aScript.BindFunction(id);

			return id;
		}

		void AddPinToFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const eFlowType aFlowType, const std::string& aPinName)
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

		void BeginFrame()
		{
			Global::Internal::GetFrameMemoryArena().Clear();
		}

		int GetPinID(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			const Pin& pin = ScriptProxy::GetPin(aNodeGraph, aPinID);
			return aNodeGraph.myMemoryArena.GetID<int>(pin.dataPtr);
		}

	}
}

