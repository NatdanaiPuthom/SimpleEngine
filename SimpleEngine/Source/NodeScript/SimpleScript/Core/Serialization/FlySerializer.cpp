#include "FlySerializer.hpp"
#include "../FlyClass.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"
#include "../Utilities/FlyLinker.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../FlyInternal.hpp"
#include "../Utilities/FlyFilter.hpp"
#include "../Command/FlyCommandTracker.hpp"
#include "../Fly.hpp"
#include "../FlyFoundation.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

constexpr const char* FILE_EXTENSION = ".fly";
constexpr const char* CUSTOM_EVENT_FILE_NAME = "CustomEvents.fly";

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

		const bool createdDirectory = std::filesystem::create_directories(std::string(aFilePath));
		if (!createdDirectory)
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

		const NodeGraph& eventGraph = aClass.GetEventGraph().mNodeGraph;
		const VariableManager& variableManager = aClass.GetVariableManager();

		nlohmann::json jsonDoc;

		jsonDoc["Name"] = aClass.Name();

		nlohmann::json& dataJson = jsonDoc["Data"];

		dataJson["Nodes"] = nlohmann::json::array();

		nlohmann::json& nodesArrayJson = dataJson["Nodes"];

		std::unordered_map<NodeID, NodeID> cleanedNodeIDs;

		for (NodeID nodeID = 0; nodeID < eventGraph.mNodes.size(); ++nodeID)
		{
			const Node& node = eventGraph.mNodes.at(nodeID);
			if (node.mIsDestroyed)
			{
				continue;
			}

			nlohmann::json nodeJson;
			cleanedNodeIDs.emplace(nodeID, static_cast<NodeID>(cleanedNodeIDs.size()));

			nodeJson["ID"] = cleanedNodeIDs.at(nodeID);
			nodeJson["Name"] = Global::GetNodeTypeManager().GetShortName(node.mTypeID);
			nodeJson["Pos"]["x"] = node.mPosition.x;
			nodeJson["Pos"]["y"] = node.mPosition.y;

			nodesArrayJson.push_back(nodeJson);
		}

		dataJson["PinData"] = nlohmann::json::array();
		nlohmann::json& pinDataArrayJson = dataJson["PinData"];

		for (const PinID inputPinID : ScriptFilter::GetInputPins(eventGraph))
		{
			const Pin& pin = eventGraph.mPins.at(inputPinID);

			nlohmann::json pinDataJson;

			pinDataJson["NodeID"] = cleanedNodeIDs.at(pin.mNodeID);
			pinDataJson["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, inputPinID);

			pinDataJson["ConnectionData"] = nlohmann::json::object();
			nlohmann::json& connectionsJson = pinDataJson["Connections"];

			const bool connectionExists = !pin.mConnectedPinIDs.empty();
			connectionsJson["Exists"] = connectionExists;

			if (connectionExists)
			{

				connectionsJson["Pins"] = nlohmann::json::array();
				nlohmann::json& connectedPinsJson = connectionsJson["Pins"];
				for (const PinID connectedPinID : pin.mConnectedPinIDs)
				{
					nlohmann::json& connectedPinJson = connectedPinsJson.emplace_back();
					const Pin& connectedPin = eventGraph.mPins.at(connectedPinID);
					connectedPinJson["NodeID"] = cleanedNodeIDs.at(connectedPin.mNodeID);
					connectedPinJson["PinIndex"] = ScriptLinker::GetPinIndex(eventGraph, connectedPinID);
				}

			}
			else
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

				pinDataJson["DataType"] = Global::GetDataTypeManager().GetName(pinType.mDataTypeID);

				nlohmann::json valueJson = nlohmann::json::object();
				Global::GetDataTypeManager().SaveData(pinType.mDataTypeID, valueJson, pin.mDataPtr);
				pinDataJson["Value"] = valueJson;
			}

			pinDataArrayJson.push_back(pinDataJson);
		}

		dataJson["Variables"] = nlohmann::json::array();
		nlohmann::json& variableDataJson = dataJson["Variables"];

		for (VarID i = 0; i < ScriptProxy::GetVariables(aClass).size(); ++i)
		{
			const Variable& variable = ScriptProxy::GetVariable(aClass, i);

			if (variable.mIsDestroyed)
			{
				continue;
			}

			nlohmann::json variableJson;

			variableJson["Name"] = variable.mName;
			variableJson["DataType"] = Global::GetDataTypeManager().GetName(variable.mDataTypeID);

			nlohmann::json defaultValueJson = nlohmann::json::object();

			Global::GetDataTypeManager().SaveData(variable.mDataTypeID, defaultValueJson, variable.mDefaultValueDataPtr);

			variableJson["DefaultValue"] = defaultValueJson;

			variableJson["Nodes"] = nlohmann::json::array();
			nlohmann::json& variableNodesJson = variableJson["Nodes"];

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

		const nlohmann::json jsonDoc = nlohmann::json::parse(file);

		NodeGraph& eventGraph = aClass.GetEventGraph().mNodeGraph;

		const nlohmann::json& dataJson = jsonDoc["Data"];

		std::unordered_set<NodeID> failedNodeIDs;

		for (const nlohmann::json& nodeJson : dataJson["Nodes"])
		{
			const std::string& nodeName = nodeJson["Name"];
			const nlohmann::json& nodePosJson = nodeJson["Pos"];
			const float xPos = nodePosJson["x"];
			const float yPos = nodePosJson["y"];

			bool success = true;
			const NodeID nodeID = Internal::CreateNode(&eventGraph, nodeName, success, { xPos, yPos }, true, nullptr);

			if (!success)
			{
				std::cout << "Failed create node with name: " + nodeName << std::endl;
				failedNodeIDs.insert(nodeID);
			}
		}

		for (const nlohmann::json& pinData : dataJson["PinData"])
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
			Pin& pin = eventGraph.mPins.at(pinID);
			const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

			const nlohmann::json& connectionJson = pinData["Connections"];
			const bool connectionExists = connectionJson["Exists"];
			if (connectionExists)
			{
				const NodeID connectionNodeID = connectionJson["NodeID"];
				if (failedNodeIDs.contains(connectionNodeID))
				{
					continue;
				}

				const size_t connectedPinIndex = connectionJson["PinIndex"];

				const PinID connectionID = ScriptLinker::GetPinID(eventGraph, connectionNodeID, connectedPinIndex, eFlowType::Output);

				if (connectionID != InvalidID<PinID>())
				{
					Internal::TryCreateLink(eventGraph, pinID, connectionID, nullptr);
				}
				continue;
			}


			const nlohmann::json& valueJson = pinData["Value"];
			Global::GetDataTypeManager().LoadData(pinType.mDataTypeID, valueJson, pin.mDataPtr);
		}

		const nlohmann::json& variableDataJson = dataJson["Variables"];

		for (const nlohmann::json& variableJson : variableDataJson)
		{
			const VarID varID = Internal::CreateVariable(aClass, GetDataTypeID<bool>(), nullptr);
			Variable& variable = aClass.GetVariableManager().mVariables.at(varID);

			const std::string& dataTypeStr = variableJson["DataType"];

			const std::string variableName = variableJson["Name"];
			SetVariableName(VariableView(varID, aClass), variableName, ClassView(aClass));

			const nlohmann::json& defaultValueJson = variableJson["DefaultValue"];


			DataTypeView dataType(Global::GetDataTypeManager().GetDataTypeIDByName(dataTypeStr));

			if (dataType)
			{

				SetVariableDataType(VariableView(varID, aClass), dataType, ClassView(aClass), nullptr);

				Global::GetDataTypeManager().LoadData(dataType.GetID(), defaultValueJson, variable.mDefaultValueDataPtr);

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
					ClassView createdClass = CreateClass(DataTypeView(GetDataTypeID<None>()), name);
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

		nlohmann::json jsonDoc = nlohmann::json::object();
		nlohmann::json customEventsJson = nlohmann::json::array();

		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();

		const std::vector<CustomEvent>& customEventNodeTypes = nodeTypeManager.GetCustomEvents();

		for (const CustomEvent& customEventNodeType : customEventNodeTypes)
		{
			const NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEventNodeType.GetExecutorTypeID());

			nlohmann::json customEventJson;

			customEventJson["Name"] = nodeTypeManager.GetShortName(customEventNodeType.GetExecutorTypeID());


			nlohmann::json pinArrayJson = nlohmann::json::array();

			for (size_t i = 1; i < executorNodeType.mNodeRecipe.mOutputPinTypeIDs.size(); ++i)
			{
				PinTypeID pinTypeID = executorNodeType.mNodeRecipe.mOutputPinTypeIDs[i];

				nlohmann::json customEventPinJson;

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

		const nlohmann::json& jsonDoc = nlohmann::json::parse(file);
		const nlohmann::json& customEventsJson = jsonDoc["CustomEvents"];

		for (const nlohmann::json& customEventJson : customEventsJson)
		{
			const std::string& nodeName = customEventJson["Name"];

			const CustomEventView customEvent = CreateCustomEvent(nodeName);

			const nlohmann::json& pinsJson = customEventJson["Pins"];

			for (const nlohmann::json& pinJson : pinsJson)
			{
				const std::string& pinName = pinJson["Name"];
				const std::string& dataTypeName = pinJson["DataType"];

				const DataTypeID dataTypeID = Global::GetDataTypeManager().GetDataTypeIDByName(dataTypeName);

				AddPinToCustomEvent(dataTypeID, customEvent.GetID(), pinName);
			}
		}
	}
}