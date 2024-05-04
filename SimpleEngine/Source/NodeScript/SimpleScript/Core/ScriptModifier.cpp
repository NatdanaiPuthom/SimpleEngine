#include "ScriptModifier.h"
#include "Script.h"
#include "Node/NodeTypeManager.h"
#include "Pin/PinTypeManager.h"
#include "Node/NodeTypeRegistry.h"
#include "ScriptManager.h"
#include "ScriptInternalModifier.h"

namespace SCR
{

	ScriptModifier::ScriptModifier(Script& aScript)
		: myScript(aScript)
	{
	}

	ScriptModifier::~ScriptModifier()
	{
	}

	NodeID ScriptModifier::CreateNode(const NodeTypeID aNodeTypeID, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Node + Set Node Pos");

		NodeID nodeID = GetInternalModifier().CreateNode(aNodeTypeID);

		SetNodePosition(nodeID, aPosition, true);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();

		return nodeID;
	}

	NodeID ScriptModifier::CreateNodeAutoLink(const NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Node + Auto Link");

		NodeID nodeID = CreateNode(aNodeTypeID, aPosition);

		const Pin& createdFromPin = ScriptProxy::GetPin(myScript, aConnection);
		const Node& createdNode = ScriptProxy::GetNode(myScript, nodeID);

		const std::vector<PinID>& pinIDs = PinTypeManager::GetPinType(createdFromPin.typeID).flowType == ePinFlowType::Input ? createdNode.outputPins : createdNode.inputPins;

		for (PinID pinID : pinIDs)
		{
			if (TryCreateLink(pinID, aConnection))
			{
				break;
			}
		}

		ScriptProxy::GetCommandTracker(myScript).EndComposite();

		return nodeID;
	}

	NodeID ScriptModifier::CreateNode(const std::string& aName, bool& aSuccess, ScriptVec2 aPosition, bool aTrackChange, bool aCreateIfNameNotFound)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
		bool prevTracking = commandTracker.IsTracking();
		commandTracker.IsTracking() = aTrackChange;
		commandTracker.BeginComposite("Create Node + Set Node Pos");
		NodeID nodeID = GetInternalModifier().CreateNode(aName, aSuccess, aCreateIfNameNotFound);

		SetNodePosition(nodeID, aPosition, aTrackChange);

		commandTracker.EndComposite();
		commandTracker.IsTracking() = prevTracking;
		return nodeID;
	}

	NodeID ScriptModifier::CreateGetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Getter Node");

		NodeID nodeID = GetInternalModifier().CreateGetterNode(aDataTypeID);
		SetNodePosition(nodeID, aPosition, true);
		GetInternalModifier().BindVariable(nodeID, aVarID);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
		return nodeID;
	}

	NodeID ScriptModifier::CreateSetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Setter Node");

		NodeID nodeID = GetInternalModifier().CreateSetterNode(aDataTypeID);
		SetNodePosition(nodeID, aPosition, true);
		GetInternalModifier().BindVariable(nodeID, aVarID);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
		return nodeID;
	}


	Link ScriptModifier::TryCreateLink(PinID aPinID1, PinID aPinID2)
	{
		Link createdLink = ScriptLinker::ArePinsLinkable(myScript, aPinID1, aPinID2);
		if (!createdLink)
		{
			// Check if we can replace node with overloaded opearator node
			const Pin& pin1 = ScriptProxy::GetPin(myScript, aPinID1);
			const Pin& pin2 = ScriptProxy::GetPin(myScript, aPinID2);

			const PinType& pinType1 = PinTypeManager::GetPinType(pin1.typeID);
			const PinType& pinType2 = PinTypeManager::GetPinType(pin2.typeID);

			if (pinType1.dataTypeID == typeid(Undefined).hash_code())
			{
				return GetInternalModifier().ReplaceOperatorNode(aPinID1, aPinID2);

			}
			else if (pinType2.dataTypeID == typeid(Undefined).hash_code())
			{
				return GetInternalModifier().ReplaceOperatorNode(aPinID2, aPinID1);
			}
			else
			{
				return Link{};
			}
		}

		GetInternalModifier().RebindLink(createdLink.inputPinID, createdLink.outputPinID);


		return Link{ aPinID2, aPinID1 };
	}

	void ScriptModifier::DestroyLink(const PinID aInputPinID)
	{
		GetInternalModifier().RebindLink(aInputPinID, InvalidID<PinID>());
	}

	void ScriptModifier::DestoryLinksByOutputPinID(const PinID aOutputPinID)
	{

		Pin& outputPin = ScriptProxy::GetPinRef(myScript, aOutputPinID);

		for (PinID connectedInputPin : outputPin.connectedPinIDs)
		{
			DestroyLink(connectedInputPin);
		}
	}

	void ScriptModifier::DestroyNode(const NodeID aNodeID)
	{

		struct DestroyNodeData
		{
			NodeID nodeID = InvalidID<NodeID>();
		} data;

		data.nodeID = aNodeID;

		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Destroy Node + Conncected Links");

		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<DestroyNodeData>>(data,
			[](const DestroyNodeData& aData, Script& aScript) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);
				node.isDestroyed = true;
				ScriptProxy::GetNodeExecutor(aScript).Unregister(aData.nodeID);

			},
			[](const DestroyNodeData& aData, Script& aScript) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);
				node.isDestroyed = false;

				ScriptProxy::GetNodeExecutor(aScript).Register(aData.nodeID);
			}, "Destroy Node"
		);

		ScriptProxy::GetInternalModifier(myScript).UnbindVariable(aNodeID);

		for (PinID inputPinID : ScriptProxy::GetNode(myScript, aNodeID).inputPins)
		{
			DestroyLink(inputPinID);
		}

		for (PinID outputPinID : ScriptProxy::GetNode(myScript, aNodeID).outputPins)
		{
			const Pin& outputPin = ScriptProxy::GetPin(myScript, outputPinID);

			for (PinID connectedInputPin : outputPin.connectedPinIDs)
			{
				DestroyLink(connectedInputPin);

			}
		}


		ScriptProxy::GetCommandTracker(myScript).EndComposite();
	}

	void ScriptModifier::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Destroy Selection");

		for (LinkID linkID : aLinkIDs)
		{
			DestroyLink(linkID);
		}

		for (NodeID nodeID : aNodeIDs)
		{
			DestroyNode(nodeID);
		}

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
	}

	void ScriptModifier::SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, bool aTrackChange)
	{
		SetNodePosition(aNodeID, aPosition, ScriptProxy::GetNode(myScript, aNodeID).position, aTrackChange);

	}
	void ScriptModifier::SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, bool aTrackChange)
	{
		struct SetNodePositionData
		{
			NodeID nodeID;
			ScriptVec2 oldPos;
			ScriptVec2 newPos;
		} data;

		if (aPosition != aOldPosition)
		{
			CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
			bool prevTracking = commandTracker.IsTracking();
			commandTracker.IsTracking() = aTrackChange;

			data.nodeID = aNodeID;
			data.oldPos = aOldPosition;
			data.newPos = aPosition;

			commandTracker.DoCommand<FunctionCommand<SetNodePositionData>>(data,
				[](const SetNodePositionData& aData, Script& aScript) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);
					node.position = aData.newPos;
				},
				[](const SetNodePositionData& aData, Script& aScript) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(aScript, aData.nodeID);
					node.position = aData.oldPos;
				}, "Set Node Position"
			);


			commandTracker.IsTracking() = prevTracking;
		}

	}

	void ScriptModifier::AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, const std::string& aName, ScriptFoundation* aFoundation)
	{
		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);


		NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.myExecutorTypeID);

		PinTypeID executorPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
		nodeType.nodeRecipe.outputPinTypeIDs.push_back(executorPinTypeID);


		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.myCallerTypeID);
		PinTypeID callerPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
		callerNodeType.nodeRecipe.inputPinTypeIDs.push_back(callerPinTypeID);

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{

				ScriptInternalModifier& internalModifier = ScriptProxy::GetInternalModifier(*script);
				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myExecutorTypeID))
				{
					PinID pinID = internalModifier.CreateOutputPin(nodeID, executorPinTypeID);

					ScriptProxy::GetNodeRef(*script, nodeID).outputPins.push_back(pinID);
				}

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myCallerTypeID))
				{
					PinID pinID = internalModifier.CreateInputPin(nodeID, callerPinTypeID);

					ScriptProxy::GetNodeRef(*script, nodeID).inputPins.push_back(pinID);
				}
			}
		}

	}

	void ScriptModifier::SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation)
	{
		if (anIndex == 0)
		{
			return;
		}
		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

		NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.myExecutorTypeID);

		if (anIndex >= nodeType.nodeRecipe.outputPinTypeIDs.size())
		{
			return;
		}

		const PinType& oldExecutorPinType = PinTypeManager::GetPinType(nodeType.nodeRecipe.outputPinTypeIDs[anIndex]);
		if (oldExecutorPinType.dataTypeID == aDataTypeID)
		{
			return;
		}

		PinTypeID newExecutorPinTypeID = PinTypeManager::Create(oldExecutorPinType.name, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
		nodeType.nodeRecipe.outputPinTypeIDs[anIndex] = newExecutorPinTypeID;



		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.myCallerTypeID);

		const PinType& oldCallerPinType = PinTypeManager::GetPinType(callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex]);

		PinTypeID newCallerPinTypeID = PinTypeManager::Create(oldCallerPinType.name, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
		callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex] = newCallerPinTypeID;

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{

				ScriptInternalModifier& internalModifier = ScriptProxy::GetInternalModifier(*script);
				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myExecutorTypeID))
				{
					PinID pinID = internalModifier.CreateOutputPin(nodeID, newExecutorPinTypeID);

					ScriptProxy::GetNodeRef(*script, nodeID).outputPins[anIndex] = pinID;

				}

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myCallerTypeID))
				{
					PinID pinID = internalModifier.CreateInputPin(nodeID, newCallerPinTypeID);

					ScriptProxy::GetNodeRef(*script, nodeID).inputPins[anIndex] = pinID;
				}
			}
		}

	}

	void ScriptModifier::DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation)
	{
		if (anIndex == 0)
		{
			return;
		}

		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

		NodeType& executorNodeType = NodeTypeManager::GetNodeType(customEvent.myExecutorTypeID);

		if (anIndex >= executorNodeType.nodeRecipe.outputPinTypeIDs.size())
		{
			return;
		}

		executorNodeType.nodeRecipe.outputPinTypeIDs.erase(executorNodeType.nodeRecipe.outputPinTypeIDs.begin() + anIndex);


		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.myCallerTypeID);
		callerNodeType.nodeRecipe.inputPinTypeIDs.erase(callerNodeType.nodeRecipe.inputPinTypeIDs.begin() + anIndex);

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{
				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myExecutorTypeID))
				{

					Node& node = ScriptProxy::GetNodeRef(*script, nodeID);
					script->GetModifier().DestoryLinksByOutputPinID(node.outputPins[anIndex]);

					node.outputPins.erase(node.outputPins.begin() + anIndex);
				}


				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(*script, customEvent.myCallerTypeID))
				{
					Node& node = ScriptProxy::GetNodeRef(*script, nodeID);

					script->GetModifier().DestroyLink(node.inputPins[anIndex]);
					node.inputPins.erase(node.inputPins.begin() + anIndex);
				}
			}
		}
	}

	void ScriptModifier::BeginNodeDrag(const std::vector<NodeDragData>& aDragData)
	{
		for (const NodeDragData& dragData : aDragData)
		{
			BeginNodeDrag(dragData.nodeID, dragData.pos);
		}
	}

	void ScriptModifier::EndNodeDrag(const std::vector<NodeDragData>& aDragData)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);

		commandTracker.BeginComposite("Move Nodes");
		for (const NodeDragData& dragData : aDragData)
		{
			EndNodeDrag(dragData.nodeID, dragData.pos);
		}
		commandTracker.EndComposite();
	}

	void ScriptModifier::BeginNodeDrag(const NodeID aNodeID, ScriptVec2 aStartPos)
	{
		MoveNodeData& data = myMoveNodesData[aNodeID];
		data.startPos = aStartPos;
	}

	void ScriptModifier::EndNodeDrag(const NodeID aNodeID, ScriptVec2 aEndPos)
	{
		if (myMoveNodesData.contains(aNodeID))
		{
			MoveNodeData& data = myMoveNodesData.at(aNodeID);
			data.endPos = aEndPos;

			SetNodePosition(aNodeID, data.endPos, data.startPos, true);

			myMoveNodesData.erase(aNodeID);
		}

	}

	VarID ScriptModifier::CreateVariable(DataTypeID aDataTypeID)
	{
		std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(myScript);
		VarID id = variables.size();
		variables.emplace_back();
		myScript.GetModifier().SetVariableDataType(id, aDataTypeID);
		return id;

	}

	void ScriptModifier::DestroyVariable(VarID aVarID)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
		commandTracker.BeginComposite("Destroy Variable + Connected Nodes");

		struct DestroyVariableData
		{
			VarID varID = InvalidID<VarID>();
		} data;

		data.varID = aVarID;

		commandTracker.DoCommand<FunctionCommand<DestroyVariableData>>(data,
			[](const DestroyVariableData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetVariableRef(aScript, aData.varID).isDestroyed = true;
			},
			[](const DestroyVariableData& aData, Script& aScript) -> void
			{
				ScriptProxy::GetVariableRef(aScript, aData.varID).isDestroyed = false;
			}
		);

		const VariableManager& variableManager = ScriptProxy::GetVariableManager(myScript);
		DestroySelection(variableManager.GetNodeIDsByVarID(aVarID), {});

		commandTracker.EndComposite();

	}

	void ScriptModifier::EditPin(PinID aPinID)
	{
		const Pin& pin = ScriptProxy::GetPin(myScript, aPinID);
		const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

		DataTypeID dataTypeID = pinType.dataTypeID;
		void* dataPtr = ScriptProxy::GetScriptMemoryPool(myScript).MemoryAt(pin.memoryID);

		MemoryPool tempPool(64);
		MemoryPoolID tempID = DataTypeManager::AllocateData(dataTypeID, tempPool);
		void* copyDataPtr = tempPool.MemoryAt(tempID);
		DataTypeManager::CopyData(dataTypeID, copyDataPtr, dataPtr);

		if (DataTypeManager::EditData(dataTypeID, dataPtr))
		{
			MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(myScript);

			MemoryPoolID prevMemoryID = DataTypeManager::AllocateData(dataTypeID, memoryPool);
			void* prevDataPtr = memoryPool.MemoryAt(prevMemoryID);
			DataTypeManager::CopyData(dataTypeID, prevDataPtr, copyDataPtr);

			CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);

			struct EditValueData
			{
				DataTypeID dataTypeID = InvalidID<DataTypeID>();
				MemoryPoolID previousMemoryID = InvalidID<MemoryPoolID>();
				MemoryPoolID actualMemoryID = InvalidID<MemoryPoolID>();
			} data;

			data.dataTypeID = dataTypeID;
			data.previousMemoryID = prevMemoryID;
			data.actualMemoryID = pin.memoryID;

			commandTracker.RegisterCommand<FunctionCommand<EditValueData>>(data,
				[](const EditValueData& aData, Script& aScript) -> void
				{
					MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aScript);

					DataTypeID dataTypeID = aData.dataTypeID;
					void* oldDataPtr = memoryPool.MemoryAt(aData.previousMemoryID);
					void* newDataPtr = memoryPool.MemoryAt(aData.actualMemoryID);

					DataTypeManager::SwapData(dataTypeID, oldDataPtr, newDataPtr);
				},
				[](const EditValueData& aData, Script& aScript) -> void
				{
					MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aScript);

					DataTypeID dataTypeID = aData.dataTypeID;
					void* oldDataPtr = memoryPool.MemoryAt(aData.previousMemoryID);
					void* newDataPtr = memoryPool.MemoryAt(aData.actualMemoryID);

					DataTypeManager::SwapData(dataTypeID, newDataPtr, oldDataPtr);
				}, "Edit Pin Data"
			);
		}
	}

	void ScriptModifier::EditVariableDefaultValue(VarID aVarID)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		MemoryPoolID id = variable.defaultValueMemoryID;

		MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(myScript);

		void* dataPtr = memoryPool.MemoryAt(id);
		if (DataTypeManager::EditData(variable.dataTypeID, dataPtr))
		{
			DataTypeManager::CopyData(variable.dataTypeID, memoryPool.MemoryAt(variable.runtimeMemoryID), dataPtr);
		}
	}

	void ScriptModifier::SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		MemoryPoolID runtimeMemID = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetScriptMemoryPool(myScript));
		MemoryPoolID defaultValueMemID = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetScriptMemoryPool(myScript));

		variable.dataTypeID = aDataTypeID;
		variable.runtimeMemoryID = runtimeMemID;
		variable.defaultValueMemoryID = defaultValueMemID;

		DestroyVariableNodes(aVarID);
	}

	void ScriptModifier::SetVariableName(VarID aVarID, const std::string& aName)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		variable.name = aName;
	}

	void ScriptModifier::DestroyVariableNodes(const VarID aVarID)
	{
		DestroySelection(ScriptProxy::GetVariableManager(myScript).GetNodeIDsByVarID(aVarID), {});
	}

	/*Selection& ScriptModifier::GetSelection()
	{
		return mySelection;
	}*/


	void ScriptModifier::CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs)
	{
		myCopyBuffer.nodeIDs = aNodeIDs;

		ScriptVec2 avgPos;
		for (NodeID nodeID : aNodeIDs)
		{
			const Node& node = ScriptProxy::GetNode(myScript, nodeID);
			avgPos += node.position;
		}
		avgPos /= static_cast<float>(aNodeIDs.size());

		myCopyBuffer.avgPosition = avgPos;
	}

	void ScriptModifier::PasteCopyBuffer(ScriptVec2 aPosition)
	{
		if (!myCopyBuffer.nodeIDs.empty())
		{
			CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
			commandTracker.BeginComposite("Paste Nodes");

			std::unordered_map<NodeID, NodeID> copyMap;

			for (NodeID nodeID : myCopyBuffer.nodeIDs)
			{
				const Node& node = ScriptProxy::GetNode(myScript, nodeID);
				ScriptVec2 newPos = aPosition + (node.position - myCopyBuffer.avgPosition);
				NodeID createdNodeID = CreateNode(node.typeID, newPos);

				
				copyMap.emplace(nodeID, createdNodeID);
			}

			for (const auto& [copiedNodeID, pastedNodeID] : copyMap)
			{
				const Node& copyNode = ScriptProxy::GetNode(myScript, copiedNodeID);
				const Node& pasteNode = ScriptProxy::GetNode(myScript, pastedNodeID);

				for (size_t i = 0; i < copyNode.inputPins.size(); ++i)
				{
					const Pin& copyInputPin = ScriptProxy::GetPin(myScript, copyNode.inputPins[i]);
					
					const PinID pasteInputPinID = pasteNode.inputPins[i];

					for (size_t j = 0; j < copyInputPin.connectedPinIDs.size(); ++j)
					{
						
						PinID copyConnectedPinID = copyInputPin.connectedPinIDs[j];
						const Pin& copyConnectedPin = ScriptProxy::GetPin(myScript, copyConnectedPinID);

						if (copyMap.contains(copyConnectedPin.nodeID))
						{
							NodeID newConnectedNodeID = copyMap.at(copyConnectedPin.nodeID);
							const Node& newConnectedNode = ScriptProxy::GetNode(myScript, newConnectedNodeID);

							PinID newConnectedPinID = newConnectedNode.outputPins[ScriptLinker::GetPinIndex(myScript, copyConnectedPinID, ePinFlowType::Output)];

							TryCreateLink(pasteInputPinID, newConnectedPinID);
						}
					}
				}
			}

			commandTracker.EndComposite();
		}
	}

	CustomEventID ScriptModifier::CreateNodeType_CustomEvent(const std::string& aName, ScriptFoundation& aFoundation)
	{
		return ScriptInternalModifier::CreateType_CustomEvent(aName, aFoundation);
	}

	ScriptInternalModifier& ScriptModifier::GetInternalModifier()
	{
		return ScriptProxy::GetInternalModifier(myScript);
	}

}
