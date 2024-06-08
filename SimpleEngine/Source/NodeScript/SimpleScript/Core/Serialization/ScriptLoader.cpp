#include "ScriptLoader.h"
#include "../Script.h"
#include "../Node/NodeManager.h"
#include "../Pin/PinManager.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Node/NodeTypeRegistry.h"
#include "../ScriptManager.h"
#include "../Utilities/ScriptLinker.h"
#include "../Pin/PinTypeManager.h"
#include "../Utilities/ScriptProxy.h"
#include "../ScriptInternalModifier.h"
#include "../Utilities/ScriptFilter.h"
#include "../Command/ScriptCommandTracker.h"
#include <fstream>
#include <nlohmann/json.hpp>

constexpr const char* CUSTOM_EVENT_FILE_NAME = "CustomEvents.json";

using json = nlohmann::json;

namespace SCR
{
	std::string ScriptLoader::SavePath = "../Source/NodeScript/data/SimpleScripts";

	void ScriptLoader::Clear()
	{
		SavePath.~basic_string();
	}

	void ScriptLoader::Save(const Script& aScript)
	{
		std::string fileDirectory = SavePath + "/";
		std::string filePath = fileDirectory + aScript.Name() + ".json";

		std::filesystem::create_directories(fileDirectory);

		std::ofstream ofs(filePath, std::ios::out);

		if (!ofs.is_open())
		{
			throw std::runtime_error("Failed to open file for writing: " + filePath);
			return;
		}

		const NodeGraph& eventGraph = ScriptProxy::GetEventGraph(aScript);
		const MemoryPool& eventGraphMemoryPool = ScriptProxy::GetGraphMemoryPool(eventGraph);
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aScript);

		json jsonDoc;

		jsonDoc["Name"] = aScript.Name();

		json& dataJson = jsonDoc["Data"];

		dataJson["Nodes"] = json::array();

		json& nodesArrayJson = dataJson["Nodes"];

		std::unordered_map<NodeID, NodeID> cleanedNodeIDs;

		for (auto& [nodeID, node, nodeType] : ScriptProxy::GetNodeManager(aScript))
		{
			if (node->isDestroyed) continue;

			json nodeJson;
			cleanedNodeIDs.emplace(nodeID, static_cast<NodeID>(cleanedNodeIDs.size()));

			nodeJson["ID"] = cleanedNodeIDs.at(nodeID);
			nodeJson["Name"] = NodeTypeManager::GetShortName(node->typeID);
			nodeJson["Pos"]["x"] = node->position.x;
			nodeJson["Pos"]["y"] = node->position.y;

			nodesArrayJson.push_back(nodeJson);
		}

		dataJson["PinData"] = json::array();
		json& pinDataArrayJson = dataJson["PinData"];

		for (PinID inputPinID : ScriptFilter::GetInputPins(eventGraph))
		{
			const Pin& pin = ScriptProxy::GetPin(eventGraph, inputPinID);

			json pinDataJson;

			pinDataJson["NodeID"] = cleanedNodeIDs.at(pin.nodeID);
			pinDataJson["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, inputPinID, ePinFlowType::Input);

			pinDataJson["Connection"] = json::object();
			bool connectionExists = !pin.connectedPinIDs.empty();
			pinDataJson["Connection"]["Exists"] = connectionExists;

			if (connectionExists)
			{
				const Pin& connectedPin = ScriptProxy::GetPin(eventGraph, pin.connectedPinIDs[0]);
				pinDataJson["Connection"]["NodeID"] = cleanedNodeIDs.at(connectedPin.nodeID);
				pinDataJson["Connection"]["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, pin.connectedPinIDs[0], ePinFlowType::Output);
			}
			else
			{
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				pinDataJson["DataType"] = DataTypeManager::GetName(pinType.dataTypeID);

				const MemoryPoolID memoryID = pin.memoryID;

				json valueJson = json::object();
				DataTypeManager::SaveData(pinType.dataTypeID, valueJson, eventGraphMemoryPool.MemoryAt(memoryID));
				pinDataJson["Value"] = valueJson;
			}

			pinDataArrayJson.push_back(pinDataJson);
		}

		dataJson["Variables"] = json::array();
		json& variableDataJson = dataJson["Variables"];

		for (VarID i = 0; i < ScriptProxy::GetVariables(aScript).size(); ++i)
		{
			const Variable& variable = ScriptProxy::GetVariable(aScript, i);

			if (variable.isDestroyed)
			{
				continue;
			}

			json variableJson;

			variableJson["Name"] = variable.name;
			variableJson["DataType"] = DataTypeManager::GetName(variable.dataTypeID);

			json defaultValueJson = json::object();
			DataTypeID dataTypeID = variable.dataTypeID;

			DataTypeManager::SaveData(dataTypeID, defaultValueJson, eventGraphMemoryPool.MemoryAt(variable.defaultValueMemoryID));

			variableJson["DefaultValue"] = defaultValueJson;

			variableJson["Nodes"] = json::array();
			json& variableNodesJson = variableJson["Nodes"];

			for (NodeID nodeID : variableManager.GetNodeIDsByVarID(i))
			{
				const Node& node = ScriptProxy::GetNode(eventGraph, nodeID);

				if (!node.isDestroyed)
				{
					NodeID cleanNodeID = cleanedNodeIDs.at(nodeID);
					variableNodesJson.push_back(cleanNodeID);
				}
			}

			variableDataJson.push_back(variableJson);
		}

		ofs << jsonDoc;
		ofs.close();
	}

	void ScriptLoader::Load(Script& aScript)
	{
		std::string filePath = SavePath + "/" + aScript.Name() + ".json";
		std::ifstream ifs(filePath);
		std::string file(
			(std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>())
		);

		if (!ifs.is_open())
		{
			throw std::runtime_error("Failed to open file for loading: " + filePath);
			return;
		}

		ifs.close();

		const json jsonDoc = json::parse(file);

		NodeGraph& eventGraph = ScriptProxy::GetEventGraph(aScript);
		MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(eventGraph);
		ScriptModifier& modifier = aScript.GetModifier();
		ScriptProxy::GetCommandTracker(aScript).IsTracking() = false;

		const json& dataJson = jsonDoc["Data"];

		std::unordered_set<NodeID> failedNodeIDs;

		for (const json& nodeJson : dataJson["Nodes"])
		{
			const std::string& nodeName = nodeJson["Name"];
			const json& nodePosJson = nodeJson["Pos"];
			float xPos = nodePosJson["x"];
			float yPos = nodePosJson["y"];

			bool success = true;
			NodeID nodeID = modifier.CreateNode(nodeName, success, { xPos, yPos }, false, true);

			if (!success)
			{
				std::cout << "Failed create node with name: " + nodeName << std::endl;
				failedNodeIDs.insert(nodeID);
			}
		}

		for (const json& pinData : dataJson["PinData"])
		{

			const NodeID nodeID = pinData["NodeID"];
			if (failedNodeIDs.contains(nodeID))
			{
				continue;
			}
			const size_t pinIndex = pinData["PinIndex"];
			const PinID pinID = ScriptLinker::GetPinID(eventGraph, nodeID, pinIndex, ePinFlowType::Input);

			if (pinID == InvalidID<PinID>())
			{
				continue;
			}
			const Pin& pin = ScriptProxy::GetPin(eventGraph, pinID);

			//InputPin& inputPin = ScriptProxy::GetInputPin(aScript, pinID);
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

			const json& connectionJson = pinData["Connection"];
			bool connectionExists = connectionJson["Exists"];
			if (connectionExists)
			{
				NodeID connectionNodeID = connectionJson["NodeID"];
				if (failedNodeIDs.contains(connectionNodeID))
				{
					continue;
				}

				size_t connectedPinIndex = connectionJson["PinIndex"];

				PinID connectionID = ScriptLinker::GetPinID(eventGraph, connectionNodeID, connectedPinIndex, ePinFlowType::Output);

				if (connectionID != InvalidID<PinID>())
				{
					modifier.TryCreateLink(pinID, connectionID);
				}
				continue;
			}


			const json& valueJson = pinData["Value"];
			DataTypeManager::LoadData(pinType.dataTypeID, valueJson, memoryPool.MemoryAt(pin.memoryID));
		}

		const json& variableDataJson = dataJson["Variables"];

		for (const json& variableJson : variableDataJson)
		{
			VarID varID = modifier.CreateVariable();

			const Variable& variable = ScriptProxy::GetVariable(aScript, varID);

			const std::string& dataTypeStr = variableJson["DataType"];

			const std::string variableName = variableJson["Name"];
			modifier.SetVariableName(varID, variableName);

			const json& defaultValueJson = variableJson["DefaultValue"];


			DataTypeID dataTypeID = DataTypeManager::GetDataTypeIDByName(dataTypeStr);

			if (dataTypeID != InvalidID<DataTypeID>())
			{
				
				aScript.GetModifier().SetVariableDataType(varID, dataTypeID);

				DataTypeManager::LoadData(dataTypeID, defaultValueJson, memoryPool.MemoryAt(variable.defaultValueMemoryID));
				DataTypeManager::LoadData(dataTypeID, defaultValueJson, memoryPool.MemoryAt(variable.runtimeMemoryID));

			}

			const json& variableNodesJson = variableJson["Nodes"];

			for (NodeID nodeID : variableNodesJson)
			{
				if (!failedNodeIDs.contains(nodeID))
				{
					InternalModifier::BindVariable(aScript, nodeID, varID);
				}
				else
				{
					std::cout << "Couldn't bind node to variable" << std::endl;
				}
			}
		}

		ScriptProxy::GetCommandTracker(aScript).IsTracking() = true;
	}

	void ScriptLoader::LoadAll(ScriptManager& aScriptManager)
	{
		aScriptManager.Clear();
		using namespace std::filesystem;

		std::string fileDirectory = SavePath + "/";

		if (!exists(fileDirectory) || !is_directory(fileDirectory))
		{
			std::cerr << "Error: Directory does not exist or is not accessible: " << fileDirectory << std::endl;
		}
		else
		{
			for (const directory_entry& entry : directory_iterator(fileDirectory))
			{

				if (is_regular_file(entry.path()))
				{
					std::cout << "Found script file: " << entry.path() << std::endl;

					std::string fileName = entry.path().filename().string();
					std::string name = fileName.substr(0, fileName.find_last_of('.'));
					Script& script = aScriptManager.CreateScript(name);

					Load(script);
				}
			}
		}

		if (aScriptManager.GetScripts().empty())
		{
			aScriptManager.CreateScript();
		}
	}

	void ScriptLoader::CreateCopy(const Script& aScript, const std::string& aCopyName)
	{
		std::string currentPath = std::filesystem::current_path().string();
		std::string filePath = currentPath + "/" + SavePath + "/" + aScript.Name() + ".json";
		std::string copyPath = currentPath + "/" + SavePath + "/" + aCopyName + ".json";

		if (std::filesystem::copy_file(filePath, copyPath))
		{
			std::cout << "Succeeded copying file: " << filePath << std::endl;
		}
		else
		{
			throw std::runtime_error("Failed copying file");
		}
	}

	void ScriptLoader::SaveCustomEvents(const std::string& aFilePath)
	{
		const std::string filePath = aFilePath + "/" + std::string(CUSTOM_EVENT_FILE_NAME);

		std::filesystem::create_directories(aFilePath);

		std::ofstream ofs(filePath, std::ios::out);

		if (!ofs.is_open())
		{
			throw std::runtime_error("Failed to open file for writing: " + filePath);
			return;
		}

		json jsonDoc = json::object();
		json customEventsJson = json::array();

		const std::vector<CustomEvent>& customEventNodeTypes = NodeTypeManager::GetCustomEvents();

		for (const CustomEvent& customEventNodeType : customEventNodeTypes)
		{
			const NodeType& executorNodeType = NodeTypeManager::GetNodeType(customEventNodeType.GetExecutorTypeID());

			json customEventJson;

			customEventJson["Name"] = NodeTypeManager::GetShortName(customEventNodeType.GetExecutorTypeID());


			json pinArrayJson = json::array();

			for (size_t i = 1; i < executorNodeType.nodeRecipe.outputPinTypeIDs.size(); ++i)
			{
				PinTypeID pinTypeID = executorNodeType.nodeRecipe.outputPinTypeIDs[i];

				json customEventPinJson;

				const PinType& pinType = PinTypeManager::GetPinType(pinTypeID);

				customEventPinJson["Name"] = pinType.name;
				customEventPinJson["DataType"] = DataTypeManager::GetName(pinType.dataTypeID);

				pinArrayJson.push_back(customEventPinJson);
			}

			customEventJson["Pins"] = pinArrayJson;

			customEventsJson.push_back(customEventJson);
		}

		jsonDoc["CustomEvents"] = customEventsJson;

		ofs << jsonDoc;

		ofs.close();
	}

	void ScriptLoader::LoadCustomEvents()
	{
		std::string filePath = SavePath + "/" + std::string(CUSTOM_EVENT_FILE_NAME);
		std::ifstream ifs(filePath);
		std::string file(
			(std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>())
		);

		if (!ifs.is_open())
		{
			return;
		}

		ifs.close();

		const json& jsonDoc = json::parse(file);
		const json& customEventsJson = jsonDoc["CustomEvents"];

		for (const json& customEventJson : customEventsJson)
		{
			const std::string& nodeName = customEventJson["Name"];

			CustomEventID customEventNodeTypeID = ScriptModifier::CreateNodeType_CustomEvent(nodeName);

			const json& pinsJson = customEventJson["Pins"];

			for (const json& pinJson : pinsJson)
			{
				const std::string& pinName = pinJson["Name"];
				const std::string& dataTypeName = pinJson["DataType"];

				DataTypeID dataTypeID = DataTypeManager::GetDataTypeIDByName(dataTypeName);

				ScriptModifier::AddPinToCustomEvent(dataTypeID, customEventNodeTypeID, pinName);
			}
		}
	}
}