#include "ScriptInternalModifier.h"
#include "Script.h"
#include "ScriptManager.h"
#include "CustomEvent/CustomEvent.h"
#include "Node/NodeTypeManager.h"
#include "Pin/PinTypeManager.h"
#include "DataType/DataTypeManager.h"
#include "Command/ScriptCommandTracker.h"
#include "Command/ScriptFunctionCommand.h"
#include "Node/NodeExecutor.h"
#include "Utilities/ScriptLinker.h"

namespace SCR
{

	CustomEventID InternalModifier::CreateCustomEvent(const std::string& aName)
	{
		CustomEvent customEvent(aName);

		std::vector<CustomEvent>& customEvents = NodeTypeManager::myCustomEvents;
		std::unordered_multimap<NodeTypeID, CustomEventID>& map = NodeTypeManager::myToCustomEventID;
		CustomEventID id = customEvents.size();
		customEvents.push_back(customEvent);
		map.emplace(customEvent.GetCallerTypeID(), id);
		map.emplace(customEvent.GetExecutorTypeID(), id);

		return id;
	}

	FunctionID InternalModifier::CreateFunction(const std::string& aName)
	{
		Function* function = new Function(aName);

		std::vector<Function*>& functions = NodeTypeManager::myFunctions;
		std::unordered_multimap<NodeTypeID, FunctionID>& map = NodeTypeManager::myToFunctionID;
		FunctionID id = functions.size();
		functions.push_back(function);
		map.emplace(function->GetCallerNodeTypeID(), id);
		map.emplace(function->GetInputNodeTypeID(), id);
		map.emplace(function->GetOutputNodeTypeID(), id);

		return id;
	}

	NodeID InternalModifier::CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker)
	{
		const NodeID id = GetCurrentNodeID(aNodeGraph);
		AddNode(aNodeGraph, NodeTypeManager::CreateInstance(aNodeGraph, id, aNodeTypeID), id, aCommandTracker);
		return id;
	}

	NodeID InternalModifier::CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker)
	{
		NodeTypeID typeID = NodeTypeManager::GetTypeID(aName);
		aSuccess = typeID != 0;
		if (!aCreateIfNameNotFound && !aSuccess)
		{
			return InvalidID<NodeID>();
		}
		return CreateNode(aNodeGraph, typeID, aCommandTracker);
	}

	NodeID InternalModifier::CreateGetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
	{
		const NodeID id = GetCurrentNodeID(aNodeGraph);
		AddNode(aNodeGraph, NodeTypeManager::CreateInstance_Getter(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
		return id;
	}

	NodeID InternalModifier::CreateSetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
	{
		const NodeID id = GetCurrentNodeID(aNodeGraph);
		AddNode(aNodeGraph, NodeTypeManager::CreateInstance_Setter(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
		return id;
	}

	NodeID InternalModifier::CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
	{
		const NodeID id = GetCurrentNodeID(aNodeGraph);
		AddNode(aNodeGraph, NodeTypeManager::CreateInstance_Operator(aNodeGraph, id, aOperatorTrait, aDataTypeID), id, aCommandTracker);
		return id;
	}

	void InternalModifier::AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* aCommandTracker)
	{
		std::vector<Node>& nodes = ScriptProxy::GetNodes(aNodeGraph);
		std::vector<std::vector<NodeID>>& nodeIDsByTypeID = ScriptProxy::GetNodeIDsByNodeTypeContainer(aNodeGraph);

		nodes.emplace_back(std::move(aNode));

		nodeIDsByTypeID.resize(NodeTypeManager::GetNodeTypes().size());
		Node& createdNode = nodes.back();
		nodeIDsByTypeID[createdNode.typeID].push_back(aNodeID);

		struct CreateNodeData
		{
			NodeID nodeID = InvalidID<NodeID>();
		} data;

		data.nodeID = aNodeID;

		auto doAction = [](const CreateNodeData& aData, const CommandContext& aContext) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);

				node.isDestroyed = false;

				ScriptProxy::GetNodeExecutor().BindToEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
			};

		if (!aCommandTracker)
		{
			doAction(data, CommandContext{ nullptr, &aNodeGraph });
		}
		else
		{
			aCommandTracker->DoCommand<FunctionCommand<CreateNodeData>>(CommandContext{ nullptr, &aNodeGraph }, data,
				doAction,
				[](const CreateNodeData& aData, const CommandContext& aContext) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);

					node.isDestroyed = true;

					ScriptProxy::GetNodeExecutor().UnbindFromEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
					ScriptProxy::GetNodeExecutor(*aContext.script).UnbindFromEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
				}, "Create Node"
			);
		}

		
	}

	PinID InternalModifier::CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
	{
		DataTypeID dataTypeID = PinTypeManager::GetPinType(aPinTypeID).dataTypeID;

		//MemoryPoolID memoryPoolID = DataTypeManager::AllocateData(dataTypeID, ScriptProxy::GetGraphMemoryPool(aNodeGraph));
		MemoryManager& memoryManager = ScriptProxy::GetNodeGraphMemoryManager(aNodeGraph);
		void* dataPtr = DataTypeManager::AllocateData(dataTypeID, memoryManager);
		return CreateInputPin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
	}

	std::vector<PinID> InternalModifier::CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
	{
		const NodeType& nodeType = NodeTypeManager::GetNodeType(aNodeTypeID);
		const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.inputPinTypeIDs;
		std::vector<PinID> pinsIDs;

		for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
		{
			pinsIDs.push_back(CreateInputPin(aNodeGraph, aNodeID, pinTypeIDs[i]));
		}

		return pinsIDs;
	}

	PinID InternalModifier::CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
	{
		DataTypeID dataTypeID = PinTypeManager::GetPinType(aPinTypeID).dataTypeID;

		MemoryManager& memoryManager = ScriptProxy::GetNodeGraphMemoryManager(aNodeGraph);

		//MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(aNodeGraph);
		void* dataPtr = DataTypeManager::AllocateData(dataTypeID, memoryManager);

		return CreateOutputPin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
	}


	std::vector<PinID> InternalModifier::CreateOutputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
	{
		const NodeType& nodeType = NodeTypeManager::GetNodeType(aNodeTypeID);
		const std::vector<PinTypeID>& pinTypeIDs = nodeType.nodeRecipe.outputPinTypeIDs;
		std::vector<PinID> pinsIDs;

		for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
		{
			pinsIDs.push_back(CreateOutputPin(aNodeGraph, aNodeID, pinTypeIDs[i]));
		}

		return pinsIDs;
	}


	PinID InternalModifier::CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, aDataPtr });
		return id;
	}

	PinID InternalModifier::CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, aDataPtr });
		return id;
	}


	/*PinID InternalModifier::CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, nullptr, aMemoryPoolID });
		return id;
	}

	PinID InternalModifier::CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID)
	{
		std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
		const PinID id = static_cast<PinID>(pins.size());
		pins.push_back(Pin{ aNodeID, aPinTypeID, nullptr, aMemoryPoolID });
		return id;
	}*/

	void InternalModifier::RebindLink(const NodeGraphContext& aContext, const PinID aInputPinID, const PinID aNewOutputPinID)
	{
		assert(aInputPinID != InvalidID<PinID>());

		struct RebindLinkData
		{
			Link createdLink;
			PinID oldOutputPinID = InvalidID<PinID>();
		} data;

		data.createdLink = { aInputPinID, aNewOutputPinID };

		const Pin& inputPin = ScriptProxy::GetPin(aContext.nodeGraph, aInputPinID);
		if (!inputPin.connectedPinIDs.empty())
		{
			data.oldOutputPinID = inputPin.connectedPinIDs[0];

		}

		ScriptProxy::GetCommandTracker(aContext.script).DoCommand<FunctionCommand<RebindLinkData>>(CommandContext{ &aContext.script, &aContext.nodeGraph}, data,
			[](const RebindLinkData& aData, const CommandContext& aContext) -> void
			{

				Pin& inputPin = ScriptProxy::GetPinRef(*aContext.nodeGraph, aData.createdLink.inputPinID);

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
					Pin& oldOutputPin = ScriptProxy::GetPinRef(*aContext.nodeGraph, aData.oldOutputPinID);
					std::erase(oldOutputPin.connectedPinIDs, aData.createdLink.inputPinID);

				}

				if (aData.createdLink.outputPinID != InvalidID<PinID>())
				{
					Pin& outputPin = ScriptProxy::GetPinRef(*aContext.nodeGraph, aData.createdLink.outputPinID);

					outputPin.connectedPinIDs.push_back(aData.createdLink.inputPinID);
				}
			},
			[](const RebindLinkData& aData, const CommandContext& aContext)
			{
				ScriptProxy::GetCommandTracker(*aContext.script).IsTracking() = false;
				InternalModifier::RebindLink(NodeGraphContext{ *aContext.nodeGraph, *aContext.script }, aData.createdLink.inputPinID, aData.oldOutputPinID);
				ScriptProxy::GetCommandTracker(*aContext.script).IsTracking() = true;
			}, "Rebind Link"
		);

	}

	void InternalModifier::UpdateNodeTypeIDSize(NodeGraph& aNodeGraph)
	{
		ScriptProxy::GetNodeIDsByNodeTypeContainer(aNodeGraph).resize(NodeTypeManager::GetNodeTypes().size());
	}

	void InternalModifier::BindVariable(Script& aScript, const NodeID aNodeID, const VarID aVarID)
	{
		struct BindVarData
		{
			NodeID nodeID = InvalidID<NodeID>();
			VarID varID = InvalidID<VarID>();
		} data;

		data.nodeID = aNodeID;
		data.varID = aVarID;
		ScriptProxy::GetCommandTracker(aScript).DoCommand<FunctionCommand<BindVarData>>(CommandContext{ &aScript }, data,
			[](const BindVarData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(*aContext.script)[aData.nodeID] = aData.varID;
			},
			[](const BindVarData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(*aContext.script).erase(aData.nodeID);

			}, "Bind Node To Variable"
		);
	}

	void InternalModifier::UnbindVariable(Script& aScript, const NodeID aNodeID)
	{
		if (!ScriptProxy::GetNodeIDToVarIDMap(aScript).contains(aNodeID))
		{
			return;
		}

		struct UnbindVarData
		{
			NodeID nodeID = InvalidID<NodeID>();
			VarID varID = InvalidID<VarID>();
		} data;

		data.nodeID = aNodeID;
		data.varID = ScriptProxy::GetNodeIDToVarIDMap(aScript).at(data.nodeID);

		ScriptProxy::GetCommandTracker(aScript).DoCommand<FunctionCommand<UnbindVarData>>(CommandContext{ &aScript }, data,
			[](const UnbindVarData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(*aContext.script).erase(aData.nodeID);
			},
			[](const UnbindVarData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetNodeIDToVarIDMap(*aContext.script)[aData.nodeID] = aData.varID;

			}
		);
	}

	Link InternalModifier::ReplaceOperatorNode(const NodeGraphContext& aContext, PinID aReplacePinID, PinID aConnectedPinID)
	{
		const Pin& replacePin = ScriptProxy::GetPin(aContext.nodeGraph, aReplacePinID);
		const Pin& connectedPin = ScriptProxy::GetPin(aContext.nodeGraph, aConnectedPinID);

		const NodeID replaceNodeID = replacePin.nodeID;
		Node& replaceNode = ScriptProxy::GetNodeRef(aContext.nodeGraph, replaceNodeID);

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
			ScriptProxy::GetCommandTracker(aContext.script).BeginComposite("Replace node composite");
			NodeID createdNodeID = CreateOperatorNode(aContext.nodeGraph, nodeType.nodeRecipe.operatorTrait, connectedPinType.dataTypeID, &ScriptProxy::GetCommandTracker(aContext.script));


			aContext.script.GetModifier().DestroyNode(replaceNodeID);

			Node& createdNode = ScriptProxy::GetNodeRef(aContext.nodeGraph, createdNodeID);
			createdNode.position = replaceNode.position;

			Link createdLink;


			{ // Link new pin
				size_t pinIndex = ScriptLinker::GetPinIndex(aContext.nodeGraph, aReplacePinID, replacePinType.flowType);

				const PinID createdPinConnectedID = replacePinType.flowType == ePinFlowType::Input ? createdNode.inputPins[pinIndex] : createdNode.outputPins[pinIndex];

				createdLink = aContext.script.GetModifier().TryCreateLink(aConnectedPinID, createdPinConnectedID);
			}

			{ // Link previously linked pins

				const Node& destroyedNode = ScriptProxy::GetNode(aContext.nodeGraph, replaceNodeID);

				for (size_t pinIndex = 0; pinIndex < destroyedNode.inputPins.size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = ScriptProxy::GetPin(aContext.nodeGraph, destroyedNode.inputPins[pinIndex]);

					if (!destroyedInputPin.connectedPinIDs.empty())
					{
						aContext.script.GetModifier().TryCreateLink(destroyedInputPin.connectedPinIDs[0], ScriptLinker::GetPinID(aContext.nodeGraph, createdNodeID, pinIndex, ePinFlowType::Input));
					}

				}

				for (size_t pinIndex = 0; pinIndex < destroyedNode.outputPins.size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = ScriptProxy::GetPin(aContext.nodeGraph, destroyedNode.outputPins.at(pinIndex));

					for (PinID connectedInputPinID : destroyedOutputPin.connectedPinIDs)
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							aContext.script.GetModifier().TryCreateLink(connectedInputPinID, ScriptLinker::GetPinID(aContext.nodeGraph, createdNodeID, pinIndex, ePinFlowType::Output));
						}

					}

				}
			}

			ScriptProxy::GetCommandTracker(aContext.script).EndComposite();
			return createdLink;
		}
		return Link{};
	}
	NodeID InternalModifier::GetCurrentNodeID(NodeGraph& aNodeGraph)
	{
		return static_cast<NodeID>(ScriptProxy::GetNodes(aNodeGraph).size());
	}
}
