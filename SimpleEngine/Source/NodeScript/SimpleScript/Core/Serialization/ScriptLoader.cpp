#include "ScriptLoader.hpp"
#include "../FlyClass.hpp"
#include "../Node/NodeManager.hpp"
#include "../Pin/PinManager.hpp"
#include "../Utilities/ScriptUtilities.hpp"
#include "../Node/NodeTypeRegistry.hpp"
#include "../Utilities/ScriptLinker.hpp"
#include "../Pin/PinTypeManager.hpp"
#include "../Utilities/ScriptProxy.hpp"
#include "../ScriptInternalModifier.hpp"
#include "../Utilities/ScriptFilter.hpp"
#include "../Command/ScriptCommandTracker.hpp"
#include "../ScriptModifier.hpp"
#include "../ScriptFoundation.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

constexpr const char* FILE_EXTENSION = ".fly";
constexpr const char* CUSTOM_EVENT_FILE_NAME = "CustomEvents.fly";

using json = nlohmann::json;

namespace FLY_NAMESPACE
{

	static std::filesystem::path GetFileDirectory(const std::string_view aFilePath)
	{
		std::string fileDirectory = std::string(aFilePath);
		const char lastChar = fileDirectory[fileDirectory.size() - 1];
		if (lastChar != '/')
		{
			fileDirectory += "/";
		}

		return fileDirectory;
	}

	void ScriptLoader::SaveClass(const Class& aClass, const std::string_view aFilePath)
	{
		std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);
		std::filesystem::path filePath = fileDirectory.string() + aClass.Name() + FILE_EXTENSION;

		if (!std::filesystem::create_directories(fileDirectory))
		{
			throw std::runtime_error("Failed to create directory: " + fileDirectory.string());
			return;

		}

		std::ofstream ofs(filePath, std::ios::out);

		if (!ofs.is_open())
		{
			throw std::runtime_error("Failed to open file for writing: " + filePath.string());
			return;
		}

		const NodeGraph& eventGraph = ScriptProxy::GetEventGraph(aClass);
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aClass);

		json jsonDoc;

		jsonDoc["Name"] = aClass.Name();

		json& dataJson = jsonDoc["Data"];

		dataJson["Nodes"] = json::array();

		json& nodesArrayJson = dataJson["Nodes"];

		std::unordered_map<NodeID, NodeID> cleanedNodeIDs;

		for (NodeID nodeID = 0; nodeID < eventGraph.mNodeManager->mNodes.size(); ++nodeID)
		{
			const Node& node = eventGraph.mNodeManager->mNodes[nodeID];
			if (node.mIsDestroyed) continue;

			json nodeJson;
			cleanedNodeIDs.emplace(nodeID, static_cast<NodeID>(cleanedNodeIDs.size()));

			nodeJson["ID"] = cleanedNodeIDs.at(nodeID);
			nodeJson["Name"] = Global::GetNodeTypeManager().GetShortName(node.mTypeID);
			nodeJson["Pos"]["x"] = node.mPosition.x;
			nodeJson["Pos"]["y"] = node.mPosition.y;

			nodesArrayJson.push_back(nodeJson);
		}

		dataJson["PinData"] = json::array();
		json& pinDataArrayJson = dataJson["PinData"];

		for (PinID inputPinID : ScriptFilter::GetInputPins(eventGraph))
		{
			const Pin& pin = ScriptProxy::GetPin(eventGraph, inputPinID);

			json pinDataJson;

			pinDataJson["NodeID"] = cleanedNodeIDs.at(pin.mNodeID);
			pinDataJson["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, inputPinID);

			pinDataJson["Connection"] = json::object();
			bool connectionExists = !pin.mConnectedPinIDs.empty();
			pinDataJson["Connection"]["Exists"] = connectionExists;

			if (connectionExists)
			{
				const Pin& connectedPin = ScriptProxy::GetPin(eventGraph, pin.mConnectedPinIDs[0]);
				pinDataJson["Connection"]["NodeID"] = cleanedNodeIDs.at(connectedPin.mNodeID);
				pinDataJson["Connection"]["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, pin.mConnectedPinIDs[0]);
			}
			else
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

				pinDataJson["DataType"] = Global::GetDataTypeManager().GetName(pinType.mDataTypeID);

				json valueJson = json::object();
				Global::GetDataTypeManager().SaveData(pinType.mDataTypeID, valueJson, pin.mDataPtr);
				pinDataJson["Value"] = valueJson;
			}

			pinDataArrayJson.push_back(pinDataJson);
		}

		dataJson["Variables"] = json::array();
		json& variableDataJson = dataJson["Variables"];

		for (VarID i = 0; i < ScriptProxy::GetVariables(aClass).size(); ++i)
		{
			const Variable& variable = ScriptProxy::GetVariable(aClass, i);

			if (variable.mIsDestroyed)
			{
				continue;
			}

			json variableJson;

			variableJson["Name"] = variable.mName;
			variableJson["DataType"] = Global::GetDataTypeManager().GetName(variable.dataTypeID);

			json defaultValueJson = json::object();
			DataTypeID dataTypeID = variable.dataTypeID;

			Global::GetDataTypeManager().SaveData(dataTypeID, defaultValueJson, variable.defaultValueDataPtr);

			variableJson["DefaultValue"] = defaultValueJson;

			variableJson["Nodes"] = json::array();
			json& variableNodesJson = variableJson["Nodes"];

			for (const NodeRef& nodeRef : variableManager.GetNodeRefsByVarID(i))
			{
				const Node& node = ScriptProxy::GetNode(*nodeRef.mNodeGraph, nodeRef.mNodeID);

				if (!node.mIsDestroyed)
				{
					const NodeID cleanNodeID = cleanedNodeIDs.at(nodeRef.mNodeID);
					variableNodesJson.push_back(cleanNodeID);
				}
			}

			variableDataJson.push_back(variableJson);
		}

		ofs << jsonDoc;
		ofs.close();
	}

	void ScriptLoader::LoadClass(Class& aClass, const std::string_view aFilePath)
	{
		const std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);
		const std::filesystem::path filePath = fileDirectory.string() + aClass.Name() + FILE_EXTENSION;
		std::ifstream ifs(filePath);
		const std::string file((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		if (!ifs.is_open())
		{
			throw std::runtime_error("Failed to open file for loading: " + filePath.string());
			return;
		}

		ifs.close();

		const json jsonDoc = json::parse(file);

		NodeGraph& eventGraph = ScriptProxy::GetEventGraph(aClass);

		const json& dataJson = jsonDoc["Data"];

		std::unordered_set<NodeID> failedNodeIDs;

		for (const json& nodeJson : dataJson["Nodes"])
		{
			const std::string& nodeName = nodeJson["Name"];
			const json& nodePosJson = nodeJson["Pos"];
			float xPos = nodePosJson["x"];
			float yPos = nodePosJson["y"];

			bool success = true;
			const NodeView node = CreateNode(eventGraph, nodeName, success, { xPos, yPos }, nullptr, true);

			if (!success)
			{
				std::cout << "Failed create node with name: " + nodeName << std::endl;
				failedNodeIDs.insert(node.GetID());
			}
		}

		for (const json& pinData : dataJson["PinData"])
		{

			const NodeID mNodeID = pinData["NodeID"];
			if (failedNodeIDs.contains(mNodeID))
			{
				continue;
			}
			const size_t pinIndex = pinData["PinIndex"];
			const PinID pinID = ScriptLinker::GetPinID(eventGraph, mNodeID, pinIndex, eFlowType::Input);

			if (pinID == InvalidID<PinID>())
			{
				continue;
			}
			const Pin& pin = ScriptProxy::GetPin(eventGraph, pinID);
			const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

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

				PinID connectionID = ScriptLinker::GetPinID(eventGraph, connectionNodeID, connectedPinIndex, eFlowType::Output);

				if (connectionID != InvalidID<PinID>())
				{
					TryCreateLink(pinID, connectionID, eventGraph, nullptr);
				}
				continue;
			}


			const json& valueJson = pinData["Value"];
			Global::GetDataTypeManager().LoadData(pinType.mDataTypeID, valueJson, pin.mDataPtr);
		}

		const json& variableDataJson = dataJson["Variables"];

		for (const json& variableJson : variableDataJson)
		{
			const VarID varID = Internal::CreateVariable(aClass, GetDataTypeID<bool>(), nullptr);
			const Variable& variable = aClass.GetVariableManager().mVariables.at(varID);

			const std::string& dataTypeStr = variableJson["DataType"];

			const std::string variableName = variableJson["Name"];
			SetVariableName(varID, variableName, ClassView(aClass));

			const json& defaultValueJson = variableJson["DefaultValue"];


			DataTypeID dataTypeID = Global::GetDataTypeManager().GetDataTypeIDByName(dataTypeStr);

			if (dataTypeID != InvalidID<DataTypeID>())
			{

				SetVariableDataType(varID, dataTypeID, ClassView(aClass), nullptr);

				Global::GetDataTypeManager().LoadData(dataTypeID, defaultValueJson, variable.defaultValueDataPtr);

			}

			/*const json& variableNodesJson = variableJson["Nodes"];

			for (const NodeID nodeID : variableNodesJson)
			{
				if (!failedNodeIDs.contains(nodeID))
				{
					InternalModifier::BindVariable(aScript, NodeRef{.nodeID = nodeID, .nodeGraph = , varID, nullptr);
				}
				else
				{
					std::cout << "Couldn't bind node to variable" << std::endl;
				}
			}*/
		}
	}

	void ScriptLoader::LoadAllClasses(const std::string_view aFilePath)
	{

		std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);

		if (!std::filesystem::exists(fileDirectory) || !std::filesystem::is_directory(fileDirectory))
		{
			std::cerr << "Error: Directory does not exist or is not accessible: " << fileDirectory << std::endl;
		}
		else
		{
			for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(fileDirectory))
			{

				if (std::filesystem::is_regular_file(entry.path()))
				{
					std::cout << "Found script file: " << entry.path() << std::endl;

					const std::string fileName = entry.path().filename().string();
					const std::string name = fileName.substr(0, fileName.find_last_of('.'));
					ClassView createdClass = CreateClass(GetDataTypeID<None>(), name);
					LoadClass(createdClass.GetClass(), aFilePath);
				}
			}
		}
	}

	void ScriptLoader::CreateCopyOfClass(const Class& aClass, const std::string_view aFilePath, const std::string_view aCopyName)
	{
		const std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);
		std::string filePath = fileDirectory.string() + aClass.Name() + FILE_EXTENSION;
		std::string copyPath = fileDirectory.string() + std::string(aCopyName) + FILE_EXTENSION;

		if (std::filesystem::copy_file(filePath, copyPath))
		{
			std::cout << "Succeeded copying file: " << filePath << std::endl;
		}
		else
		{
			throw std::runtime_error("Failed copying file");
		}
	}

	void ScriptLoader::SaveCustomEvents(const std::string_view aFilePath)
	{
		const std::filesystem::path filePath = GetFileDirectory(aFilePath).string() + CUSTOM_EVENT_FILE_NAME;

		if (!std::filesystem::create_directories(aFilePath))
		{
			throw std::runtime_error("Failed to create directory for writing: " + filePath.string());
			return;
		}

		std::ofstream ofs(filePath, std::ios::out);

		if (!ofs.is_open())
		{
			throw std::runtime_error("Failed to open file for writing: " + filePath.string());
			return;
		}

		json jsonDoc = json::object();
		json customEventsJson = json::array();

		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();

		const std::vector<CustomEvent>& customEventNodeTypes = nodeTypeManager.GetCustomEvents();

		for (const CustomEvent& customEventNodeType : customEventNodeTypes)
		{
			const NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEventNodeType.GetExecutorTypeID());

			json customEventJson;

			customEventJson["Name"] = nodeTypeManager.GetShortName(customEventNodeType.GetExecutorTypeID());


			json pinArrayJson = json::array();

			for (size_t i = 1; i < executorNodeType.mNodeRecipe.mOutputPinTypeIDs.size(); ++i)
			{
				PinTypeID pinTypeID = executorNodeType.mNodeRecipe.mOutputPinTypeIDs[i];

				json customEventPinJson;

				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pinTypeID);

				customEventPinJson["Name"] = pinType.mName;
				customEventPinJson["DataType"] = Global::GetDataTypeManager().GetName(pinType.mDataTypeID);

				pinArrayJson.push_back(customEventPinJson);
			}

			customEventJson["Pins"] = pinArrayJson;

			customEventsJson.push_back(customEventJson);
		}

		jsonDoc["CustomEvents"] = customEventsJson;

		ofs << jsonDoc;

		ofs.close();
	}

	void ScriptLoader::LoadCustomEvents(const std::string_view aFilePath)
	{
		const std::string filePath = GetFileDirectory(aFilePath).string() + CUSTOM_EVENT_FILE_NAME;
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

			const CustomEventView customEvent = CreateCustomEvent(nodeName);

			const json& pinsJson = customEventJson["Pins"];

			for (const json& pinJson : pinsJson)
			{
				const std::string& pinName = pinJson["Name"];
				const std::string& dataTypeName = pinJson["DataType"];

				const DataTypeID dataTypeID = Global::GetDataTypeManager().GetDataTypeIDByName(dataTypeName);

				AddPinToCustomEvent(dataTypeID, customEvent.GetID(), pinName);
			}
		}
	}
}