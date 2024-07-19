#include "ScriptInternalModifier.h"
#include "Script.h"
#include "CustomEvent/CustomEvent.h"
#include "Node/NodeTypeManager.h"
#include "Pin/PinTypeManager.h"
#include "DataType/DataTypeManager.h"
#include "Command/ScriptCommandTracker.h"
#include "Command/ScriptFunctionCommand.h"
#include "Node/NodeExecutor.h"
#include "Utilities/ScriptLinker.h"
#include "ScriptModifier.h"
#include "Global/ScriptGlobal.h"
#include "ScriptFlow.h"

namespace SCR
{

	namespace Internal
	{


		CustomEventID CreateCustomEvent(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateFunction(aName);
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, NodeTypeManager::GetInstance().CreateInstance(aNodeGraph, id, aNodeTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker)
		{
			const NodeTypeID typeID = NodeTypeManager::GetInstance().GetTypeID(aName);
			aSuccess = typeID != 0;
			if (!aCreateIfNameNotFound && !aSuccess)
			{
				return InvalidID<NodeID>();
			}
			return CreateNode(aNodeGraph, typeID, aCommandTracker);
		}

		NodeID CreateGetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, NodeTypeManager::GetInstance().CreateInstance_Getter(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateSetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, NodeTypeManager::GetInstance().CreateInstance_Setter(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, NodeTypeManager::GetInstance().CreateInstance_Operator(aNodeGraph, id, aOperatorTrait, aDataTypeID), id, aCommandTracker);
			return id;
		}

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* aCommandTracker)
		{
			std::vector<Node>& nodes = ScriptProxy::GetNodes(aNodeGraph);

			nodes.emplace_back(std::move(aNode));

			Node& createdNode = nodes.back();

			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(createdNode.typeID);
			nodeType.nodeRefs.push_back(NodeRef{ .nodeID = aNodeID, .nodeGraph = &aNodeGraph });

			struct CreateNodeData
			{
				NodeID nodeID = InvalidID<NodeID>();
				NodeGraph* nodeGraph = nullptr;
			} data;

			data.nodeID = aNodeID;
			data.nodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(*data.nodeGraph, data.nodeID);

					node.isDestroyed = aCommandType == eCommandType::Undo;
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Create Node"));
			}
		}

		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
		{
			const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.inputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
		{
			DataTypeID dataTypeID = PinTypeManager::GetPinType(aPinTypeID).dataTypeID;

			MemoryArena<NodeBufferCapacity>& memoryArena = ScriptProxy::GetNodeGraphMemoryArena(aNodeGraph);

			void* dataPtr = Global::GetDataTypeManager().AllocateData(dataTypeID, memoryArena);

			return CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
		}


		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
		{
			const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.outputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}


		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
		{
			std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
			const PinID id = static_cast<PinID>(pins.size());
			pins.push_back(Pin{ aNodeID, aPinTypeID, aDataPtr });
			return id;
		}

		void ActivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.myLinks[aLinkID];


			{
				Pin& inputPin = ScriptProxy::GetPinRef(aNodeGraph, link.inputPinID);
				auto it = std::find(inputPin.connectedPinIDs.begin(), inputPin.connectedPinIDs.end(), link.outputPinID);
				if (it == inputPin.connectedPinIDs.end())
				{
					inputPin.connectedPinIDs.push_back(link.outputPinID);
				}
			}
			{
				Pin& outputPin = ScriptProxy::GetPinRef(aNodeGraph, link.outputPinID);
				auto it = std::find(outputPin.connectedPinIDs.begin(), outputPin.connectedPinIDs.end(), link.inputPinID);
				if (it == outputPin.connectedPinIDs.end())
				{
					outputPin.connectedPinIDs.push_back(link.inputPinID);
				}
			}

			link.isDestroyed = false;
		}

		void DeactivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.myLinks[aLinkID];

			Pin& inputPin = ScriptProxy::GetPinRef(aNodeGraph, link.inputPinID);
			Pin& outputPin = ScriptProxy::GetPinRef(aNodeGraph, link.outputPinID);

			std::erase(inputPin.connectedPinIDs, link.outputPinID);
			std::erase(outputPin.connectedPinIDs, link.inputPinID);

			link.isDestroyed = true;
		}

		LinkID CreateLink(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID, CommandTracker* aCommandTracker)
		{
			assert(aInputPinID != InvalidID<PinID>());
			assert(aOutputPinID != InvalidID<PinID>());

			struct CreateLinkData
			{
				LinkID createdLinkID = InvalidID<LinkID>();
				LinkID previousLinkID = InvalidID<LinkID>();
				NodeGraph* nodeGraph = nullptr;
			} data;

			data.nodeGraph = &aNodeGraph;
			data.createdLinkID = static_cast<LinkID>(aNodeGraph.myLinks.size());


			const Pin& inputPin = ScriptProxy::GetPin(aNodeGraph, aInputPinID);
			const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, aOutputPinID);
			const PinType& inputPinType = Global::GetPinTypeManager().GetPinType(inputPin.typeID);
			const PinType& outputPinType = Global::GetPinTypeManager().GetPinType(outputPin.typeID);
			DataTypeID inputPinDataType = inputPinType.dataTypeID;
			DataTypeID outputPinDataType = outputPinType.dataTypeID;
			assert(inputPinType.flowType == eFlowType::Input);
			assert(outputPinType.flowType == eFlowType::Output);
			assert(inputPinDataType == outputPinDataType);

			if (inputPinDataType != GetDataTypeID<Flow>())
			{
				std::vector<LinkID> inputLinkIDs = ScriptLinker::GetLinkIDsByPin(aNodeGraph, aInputPinID);
				if (!inputLinkIDs.empty())
				{
					assert(inputLinkIDs.size() == 1);
					data.previousLinkID = inputLinkIDs.front();
				}
			}
			else
			{
				std::vector<LinkID> outputLinkIDs = ScriptLinker::GetLinkIDsByPin(aNodeGraph, aOutputPinID);

				if (!outputLinkIDs.empty())
				{
					assert(outputLinkIDs.size() == 1);
					data.previousLinkID = outputLinkIDs.front();
				}

			}

			aNodeGraph.myLinks.push_back(Link{ aInputPinID, aOutputPinID });

			auto doAction = [](const CreateLinkData& aData) -> void
				{
					if (aData.previousLinkID != InvalidID<LinkID>())
					{
						DeactivateLink(*aData.nodeGraph, aData.previousLinkID);
					}

					ActivateLink(*aData.nodeGraph, aData.createdLinkID);
				};

			auto commandFunction = [data](eCommandType aCommandType)
				{
					if (aCommandType == eCommandType::Do)
					{

						if (data.previousLinkID != InvalidID<LinkID>())
						{
							DeactivateLink(*data.nodeGraph, data.previousLinkID);
						}

						ActivateLink(*data.nodeGraph, data.createdLinkID);
					}
					else
					{
						DeactivateLink(*data.nodeGraph, data.createdLinkID);
						if (data.previousLinkID != InvalidID<LinkID>())
						{
							ActivateLink(*data.nodeGraph, data.previousLinkID);
						}
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Create Link"));
			}

			return data.createdLinkID;
		}


		void DestroyLink(NodeGraph& aNodeGraph, const LinkID aLinkID, CommandTracker* aCommandTracker)
		{
			assert(aLinkID != InvalidID<LinkID>());

			struct DestroyLinkData
			{
				LinkID destroyedLinkID = InvalidID<LinkID>();
				NodeGraph* nodeGraph = nullptr;
			} data;

			data.destroyedLinkID = aLinkID;
			data.nodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					void (*func) (NodeGraph&, LinkID) = aCommandType == eCommandType::Do ? DeactivateLink : ActivateLink;

					func(*data.nodeGraph, data.destroyedLinkID);
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Destory Link"));
			}
		}

		VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(aScript);
			const VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(id, aDataTypeID, aScript, aCommandTracker);
			return id;
		}

		void BindVariable(Script& aScript, const NodeRef& aNodeRef, const VarID aVarID, CommandTracker* aCommandTracker)
		{
			struct BindVarData
			{
				NodeRef nodeRef;
				VarID varID = InvalidID<VarID>();
				Script* script = nullptr;
			} data;

			data.nodeRef = aNodeRef;
			data.varID = aVarID;
			data.script = &aScript;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script)[data.nodeRef] = data.varID;
					}
					else
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script).erase(data.nodeRef);
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Bind Node To Variable"));
			}
		}

		void UnbindVariable(Script& aScript, const NodeRef& aNodeRef, CommandTracker* aCommandTracker)
		{
			if (!ScriptProxy::GetNodeRefToVarIDMap(aScript).contains(aNodeRef))
			{
				return;
			}

			struct UnbindVarData
			{
				NodeRef nodeRef;
				VarID varID = InvalidID<VarID>();
				Script* script = nullptr;
			} data;

			data.nodeRef = aNodeRef;
			data.varID = ScriptProxy::GetNodeRefToVarIDMap(aScript).at(data.nodeRef);
			data.script = &aScript;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script).erase(data.nodeRef);
					}
					else
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script)[data.nodeRef] = data.varID;
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Unbind Variable"));
			}
		}

		void ReplaceOperatorNode(NodeGraph& aNodeGraph, PinID aReplacePinID, PinID aConnectedPinID, CommandTracker* aCommandTracker)
		{
			const Pin& replacePin = ScriptProxy::GetPin(aNodeGraph, aReplacePinID);
			const Pin& connectedPin = ScriptProxy::GetPin(aNodeGraph, aConnectedPinID);

			const NodeID replaceNodeID = replacePin.nodeID;
			Node& replaceNode = ScriptProxy::GetNodeRef(aNodeGraph, replaceNodeID);

			const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(replaceNode.typeID);

			if (nodeType.nodeRecipe.operatorTrait != eNodeOperatorTrait::None)
			{
				const PinType& replacePinType = PinTypeManager::GetPinType(replacePin.typeID);
				const PinType& connectedPinType = PinTypeManager::GetPinType(connectedPin.typeID);
				bool canReplace = NodeTypeManager::GetInstance().CanCreateOperatorNode(nodeType.nodeRecipe.operatorTrait, connectedPinType.dataTypeID);
				if (!canReplace)
				{
					return;
				}
				if (aCommandTracker)
				{
					aCommandTracker->BeginComposite("Replace node composite");
				}
				const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, nodeType.nodeRecipe.operatorTrait, connectedPinType.dataTypeID, aCommandTracker);


				DestroyNode(replaceNodeID, aNodeGraph, aCommandTracker);

				Node& createdNode = ScriptProxy::GetNodeRef(aNodeGraph, createdNodeID);
				createdNode.position = replaceNode.position;

				Link createdLink;


				{ // Link new pin
					size_t pinIndex = ScriptLinker::GetPinIndex(aNodeGraph, aReplacePinID);

					const PinID createdPinConnectedID = replacePinType.flowType == eFlowType::Input ? createdNode.inputPins[pinIndex] : createdNode.outputPins[pinIndex];

					TryCreateLink(aConnectedPinID, createdPinConnectedID, aNodeGraph, aCommandTracker);
				}

				{ // Link previously linked pins

					const Node& destroyedNode = ScriptProxy::GetNode(aNodeGraph, replaceNodeID);

					for (size_t pinIndex = 0; pinIndex < destroyedNode.inputPins.size(); ++pinIndex)
					{
						const Pin& destroyedInputPin = ScriptProxy::GetPin(aNodeGraph, destroyedNode.inputPins[pinIndex]);

						if (!destroyedInputPin.connectedPinIDs.empty())
						{
							TryCreateLink(destroyedInputPin.connectedPinIDs[0], ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Input), aNodeGraph, aCommandTracker);
						}

					}

					for (size_t pinIndex = 0; pinIndex < destroyedNode.outputPins.size(); ++pinIndex)
					{
						const Pin& destroyedOutputPin = ScriptProxy::GetPin(aNodeGraph, destroyedNode.outputPins.at(pinIndex));

						for (PinID connectedInputPinID : destroyedOutputPin.connectedPinIDs)
						{
							if (connectedInputPinID != InvalidID<PinID>())
							{
								TryCreateLink(connectedInputPinID, ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Output), aNodeGraph, aCommandTracker);
							}
						}
					}
				}

				if (aCommandTracker)
				{
					aCommandTracker->EndComposite();
				}
			}
		}

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph)
		{
			return static_cast<NodeID>(ScriptProxy::GetNodes(aNodeGraph).size());
		}
	}
}
