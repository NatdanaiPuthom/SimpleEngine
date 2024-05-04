#include "ScriptInternalModifier.h"
#include "Script.h"
#include "ScriptManager.h"
#include "CustomEvent/CustomEvent.h"
#include "Node/NodeTypeManager.h"

namespace SCR
{
	ScriptInternalModifier::ScriptInternalModifier(Script& aScript)
		: myScript(aScript)
	{
	}

	ScriptInternalModifier::~ScriptInternalModifier()
	{
	}

	CustomEventID ScriptInternalModifier::CreateType_CustomEvent(const std::string& aName, ScriptFoundation& aFoundation)
	{
		CustomEvent* customEvent = new CustomEvent(aName);

		std::vector<CustomEvent*>& customEvents = NodeTypeManager::myCustomEvents;
		std::unordered_multimap<NodeTypeID, CustomEventID>& map = NodeTypeManager::myToCustomEventNodeTypeID;
		CustomEventID id = customEvents.size();
		customEvents.push_back(customEvent);
		map.emplace(customEvent->myCallerTypeID, id);

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{
				ScriptProxy::GetInternalModifier(*script).UpdateNodeTypeIDSize();

			}
		}
		return id;
	}

	NodeID ScriptInternalModifier::CreateNode(const NodeTypeID aNodeTypeID)
	{
		const NodeID id = GetCurrentNodeID();
		AddNode(NodeTypeManager::CreateInstance(id, aNodeTypeID, *this), id);
		return id;
	}

	NodeID ScriptInternalModifier::CreateNode(const std::string& aName, bool& aSuccess, bool aCreateIfNameNotFound)
	{
		NodeTypeID typeID = NodeTypeManager::GetTypeID(aName);
		aSuccess = typeID != 0;
		if (!aCreateIfNameNotFound && !aSuccess)
		{
			return InvalidID<NodeID>();
		}
		return CreateNode(typeID);
	}

	NodeID ScriptInternalModifier::CreateGetterNode(const DataTypeID aDataTypeID)
	{
		const NodeID id = GetCurrentNodeID();
		AddNode(NodeTypeManager::CreateInstance_Getter(id, aDataTypeID, *this), id);
		return id;
	}

	NodeID ScriptInternalModifier::CreateSetterNode(const DataTypeID aDataTypeID)
	{
		const NodeID id = GetCurrentNodeID();
		AddNode(NodeTypeManager::CreateInstance_Setter(id, aDataTypeID, *this), id);
		return id;
	}

	NodeID ScriptInternalModifier::CreateOperatorNode(const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID)
	{
		const NodeID id = GetCurrentNodeID();
		AddNode(NodeTypeManager::CreateInstance_Operator(id, aOperatorTrait, aDataTypeID, *this), id);
		return id;
	}

	void ScriptInternalModifier::AddNode(Node&& aNode, const NodeID aNodeID)
	{
		std::vector<Node>& nodes = ScriptProxy::GetNodes(myScript);
		std::vector<std::vector<NodeID>>& nodeIDByTypeIDs = ScriptProxy::GetNodeIDsByNodeTypeContainer(myScript);

		nodes.emplace_back(std::move(aNode));

		Node& createdNode = nodes.back();
		nodeIDByTypeIDs.at(createdNode.typeID).push_back(aNodeID);

		struct CreateNodeData
		{
			NodeID nodeID = InvalidID<NodeID>();
		} data;

		data.nodeID = aNodeID;

		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<CreateNodeData>>(data,
			[](const CreateNodeData& aData, Script& aScript) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);

				node.isDestroyed = false;

				ScriptProxy::GetNodeExecutor(aScript).Register(aData.nodeID);
			},
			[](const CreateNodeData& aData, Script& aScript) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);

				node.isDestroyed = true;

				ScriptProxy::GetNodeExecutor(aScript).Unregister(aData.nodeID);
			}, "Create Node"
		);
	}

	PinID ScriptInternalModifier::CreateInputPin(const NodeID aNodeID, const PinTypeID aPinTypeID)
	{
		DataTypeID dataTypeID = PinTypeManager::GetPinType(aPinTypeID).dataTypeID;

		MemoryPoolID memoryPoolID = DataTypeManager::AllocateData(dataTypeID, ScriptProxy::GetScriptMemoryPool(myScript));

		return CreateInputPin(aNodeID, aPinTypeID, memoryPoolID);
	}

	std::vector<PinID> ScriptInternalModifier::CreateInputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
	{
		const NodeType& nodeType = NodeTypeManager::GetNodeType(aNodeTypeID);
		const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.inputPinTypeIDs;
		std::vector<PinID> pinsIDs;

		for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
		{
			pinsIDs.push_back(CreateInputPin(aNodeID, pinTypeIDs[i]));
		}

		return pinsIDs;
	}

	PinID ScriptInternalModifier::CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID)
	{
		DataTypeID dataTypeID = PinTypeManager::GetPinType(aPinTypeID).dataTypeID;

		MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(myScript);
		MemoryPoolID memoryID = DataTypeManager::AllocateData(dataTypeID, memoryPool);

		return CreateOutputPin(aNodeID, aPinTypeID, memoryID);
	}


	std::vector<PinID> ScriptInternalModifier::CreateOutputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
	{
		const NodeType& nodeType = NodeTypeManager::GetNodeType(aNodeTypeID);
		const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.outputPinTypeIDs;
		std::vector<PinID> pinsIDs;

		for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
		{
			pinsIDs.push_back(CreateOutputPin(aNodeID, pinTypeIDs[i]));
		}

		return pinsIDs;
	}


	PinID ScriptInternalModifier::CreateInputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(myScript);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, aMemoryPoolID });
		return id;
	}

	PinID ScriptInternalModifier::CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(myScript);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, aMemoryPoolID });
		return id;
	}

	void ScriptInternalModifier::RebindLink(const PinID aInputPinID, const PinID aNewOutputPinID)
	{
		assert(aInputPinID != InvalidID<PinID>());

		struct RebindLinkData
		{
			Link createdLink;
			PinID oldOutputPinID = InvalidID<PinID>();
		} data;

		data.createdLink = { aInputPinID, aNewOutputPinID };

		const Pin& inputPin = ScriptProxy::GetPin(myScript, aInputPinID);
		if (!inputPin.connectedPinIDs.empty())
		{
			data.oldOutputPinID = inputPin.connectedPinIDs[0];

		}

		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<RebindLinkData>>(data,
			[](const RebindLinkData& aData, Script& aScript) -> void
			{

				Pin& inputPin = ScriptProxy::GetPinRef(aScript, aData.createdLink.inputPinID);

				if (aData.createdLink.outputPinID == InvalidID<PinID>())
				{
					inputPin.connectedPinIDs.clear();
				}
				else
				{
					inputPin.connectedPinIDs.resize(1);
					inputPin.connectedPinIDs[0] = aData.createdLink.outputPinID;
				}

				if (aData.oldOutputPinID != InvalidID<PinID>())
				{
					Pin& oldOutputPin = ScriptProxy::GetPinRef(aScript, aData.oldOutputPinID);
					std::erase(oldOutputPin.connectedPinIDs, aData.createdLink.inputPinID);

				}

				if (aData.createdLink.outputPinID != InvalidID<PinID>())
				{
					Pin& outputPin = ScriptProxy::GetPinRef(aScript, aData.createdLink.outputPinID);

					outputPin.connectedPinIDs.push_back(aData.createdLink.inputPinID);
				}
			},
			[](const RebindLinkData& aData, Script& aScript)
			{
				ScriptProxy::GetCommandTracker(aScript).IsTracking() = false;
				ScriptProxy::GetInternalModifier(aScript).RebindLink(aData.createdLink.inputPinID, aData.oldOutputPinID);
				ScriptProxy::GetCommandTracker(aScript).IsTracking() = true;
			}, "Rebind Link"
		);

	}

	void ScriptInternalModifier::UpdateNodeTypeIDSize()
	{
		ScriptProxy::GetNodeIDsByNodeTypeContainer(myScript).resize(NodeTypeManager::GetNodeTypes().size());
	}

	void ScriptInternalModifier::BindVariable(const NodeID aNodeID, const VarID aVarID)
	{
		struct BindVarData
		{
			NodeID nodeID = InvalidID<NodeID>();
			VarID varID = InvalidID<VarID>();
		} data;

		data.nodeID = aNodeID;
		data.varID = aVarID;
		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<BindVarData>>(data,
			[](const BindVarData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(aScript)[aData.nodeID] = aData.varID;
			},
			[](const BindVarData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(aScript).erase(aData.nodeID);

			}, "Bind Node To Variable"
		);
	}

	void ScriptInternalModifier::UnbindVariable(const NodeID aNodeID)
	{
		if (!ScriptProxy::GetNodeIDToVarIDMap(myScript).contains(aNodeID))
		{
			return;
		}

		struct UnbindVarData
		{
			NodeID nodeID = InvalidID<NodeID>();
			VarID varID = InvalidID<VarID>();
		} data;

		data.nodeID = aNodeID;
		data.varID = ScriptProxy::GetNodeIDToVarIDMap(myScript).at(data.nodeID);

		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<UnbindVarData>>(data,
			[](const UnbindVarData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(aScript).erase(aData.nodeID);
			},
			[](const UnbindVarData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(aScript)[aData.nodeID] = aData.varID;

			}
		);
	}

	Link ScriptInternalModifier::ReplaceOperatorNode(PinID aReplacePinID, PinID aConnectedPinID)
	{
		const Pin& replacePin = ScriptProxy::GetPin(myScript, aReplacePinID);
		const Pin& connectedPin = ScriptProxy::GetPin(myScript, aConnectedPinID);

		const NodeID replaceNodeID = replacePin.nodeID;
		Node& replaceNode = ScriptProxy::GetNodeRef(myScript, replaceNodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(replaceNode.typeID);

		if (nodeType.nodeRecipe.operatorTrait != eNodeOperatorTrait::None)
		{
			const PinType& replacePinType = PinTypeManager::GetPinType(replacePin.typeID);
			const PinType& connectedPinType = PinTypeManager::GetPinType(connectedPin.typeID);
			bool canReplace = NodeTypeManager::CanCreateOperatorNode(nodeType.nodeRecipe.operatorTrait, connectedPinType.dataTypeID);
			if (!canReplace)
			{
				return Link{};
			}
			ScriptProxy::GetCommandTracker(myScript).BeginComposite("Replace node composite");
			NodeID createdNodeID = CreateOperatorNode(nodeType.nodeRecipe.operatorTrait, connectedPinType.dataTypeID);


			myScript.GetModifier().DestroyNode(replaceNodeID);

			Node& createdNode = ScriptProxy::GetNodeRef(myScript, createdNodeID);
			createdNode.position = replaceNode.position;

			Link createdLink;


			{ // Link new pin
				size_t pinIndex = ScriptLinker::GetPinIndex(myScript, aReplacePinID, replacePinType.flowType);

				const PinID createdPinConnectedID = replacePinType.flowType == ePinFlowType::Input ? createdNode.inputPins[pinIndex] : createdNode.outputPins[pinIndex];

				createdLink = myScript.GetModifier().TryCreateLink(aConnectedPinID, createdPinConnectedID);
			}

			{ // Link previously linked pins

				const Node& destroyedNode = ScriptProxy::GetNode(myScript, replaceNodeID);

				for (size_t pinIndex = 0; pinIndex < destroyedNode.inputPins.size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = ScriptProxy::GetPin(myScript, destroyedNode.inputPins[pinIndex]);

					if (!destroyedInputPin.connectedPinIDs.empty())
					{
						myScript.GetModifier().TryCreateLink(destroyedInputPin.connectedPinIDs[0], ScriptLinker::GetPinID(myScript, createdNodeID, pinIndex, ePinFlowType::Input));
					}

				}

				for (size_t pinIndex = 0; pinIndex < destroyedNode.outputPins.size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = ScriptProxy::GetPin(myScript, destroyedNode.outputPins.at(pinIndex));

					for (PinID connectedInputPinID : destroyedOutputPin.connectedPinIDs)
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							myScript.GetModifier().TryCreateLink(connectedInputPinID, ScriptLinker::GetPinID(myScript, createdNodeID, pinIndex, ePinFlowType::Output));
						}

					}

				}
			}

			ScriptProxy::GetCommandTracker(myScript).EndComposite();
			return createdLink;
		}
		return Link{};
	}
	NodeID ScriptInternalModifier::GetCurrentNodeID() const
	{
		return static_cast<NodeID>(ScriptProxy::GetNodes(myScript).size());
	}
}
