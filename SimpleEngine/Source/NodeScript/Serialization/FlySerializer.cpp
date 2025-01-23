#include "FlySerializer.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Command/FlyCommandTracker.hpp"
#include "../Internal/FlyFoundation.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

constexpr const char* FLY_FILE_EXTENSION = ".fly";
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

		return std::filesystem::path(std::filesystem::current_path().string() + "/" + fileDirectory);
	}

	namespace Internal
	{

		void LoadFlyFile(std::string_view aFilePath)
		{
			const std::filesystem::path filePath = aFilePath;
			std::ifstream ifs(filePath);
			const std::string file((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

			if (!ifs.is_open())
			{
				throw std::runtime_error("Failed to open file for loading: " + filePath.string());
				return;
			}

			ifs.close();

			const nlohmann::json jsonDoc = nlohmann::json::parse(file);

			const std::string& flyType = jsonDoc["Type"];

			if (flyType == "Class")
			{
				LoadClass(jsonDoc);
			}
			else if (flyType == "Struct")
			{
				LoadDataType(jsonDoc);
			}
		}


		void SaveDataType(const DataType& aDataType, std::string_view aFilePath)
		{
			std::filesystem::path fileDirectory = aFilePath;
			std::filesystem::path filePath = fileDirectory.string() + "/" + aDataType.mName + FLY_FILE_EXTENSION;

			std::filesystem::create_directories(std::string(aFilePath));
			if (!std::filesystem::exists(fileDirectory))
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


			nlohmann::json jsonDoc;

			jsonDoc["Type"] = "Struct";
			jsonDoc["Name"] = aDataType.mName;

			nlohmann::json& dataJson = jsonDoc["Data"];

			{
				dataJson["Variables"] = nlohmann::json::array();
				nlohmann::json& variableDataJson = dataJson["Variables"];

				for (VarID varID{ 0 }; varID < aDataType.mVariableContainer.mVariables.size(); ++varID)
				{
					const Variable& variable = aDataType.mVariableContainer.mVariables.at(varID);

					if (variable.mIsDestroyed)
					{
						continue;
					}

					nlohmann::json variableJson;

					variableJson["Name"] = variable.mName;
					variableJson["DataType"] = GetDataTypeManager().GetName(variable.mDataTypeID);

					nlohmann::json defaultValueJson = nlohmann::json::object();

					GetDataTypeManager().SaveData(variable.mDataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

					variableJson["DefaultValue"] = defaultValueJson;

					/*variableJson["Nodes"] = nlohmann::json::array();
					nlohmann::json& variableNodesJson = variableJson["Nodes"];

					for (const NodeRef& nodeRef : variableManager.GetNodeRefsByVarID(varID))
					{

						const Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

						if (!node.mIsDestroyed)
						{
							nlohmann::json& varNodeJson = variableNodesJson.emplace_back();
							const NodeID cleanNodeID = cleanedNodeIDs.at(nodeRef.GetNodeID());
							varNodeJson["NodeID"] = cleanNodeID;
							nlohmann::json& graphJson = varNodeJson["Graph"];
							assert(nodeRef.GetClass());
							graphJson["ClassName"] = nodeRef.GetClass()->mName;
							graphJson
							variableNodesJson.push_back(varNodeJson);
						}
					}*/

					variableDataJson.push_back(variableJson);
				}
			}

			ofs << jsonDoc;
		}


		void LoadDataType(const nlohmann::json& aJsonData, DataType& aDataType)
		{
			const nlohmann::json& dataJson = aJsonData;


			const nlohmann::json& variableDataJson = dataJson["Variables"];

			for (const nlohmann::json& variableJson : variableDataJson)
			{
				const VarID varID = Internal::CreateVariable(aDataType.mVariableContainer, GenericDataTypeID{ GetDataTypeID<bool>() }, "Var", nullptr);
				Variable& variable = aDataType.mVariableContainer.mVariables.at(varID);

				const std::string& dataTypeStr = variableJson["DataType"];

				const std::string variableName = variableJson["Name"];
				Internal::SetVariableName(varID, aDataType.mVariableContainer, variableName, nullptr);

				const nlohmann::json& defaultValueJson = variableJson["DefaultValue"];


				const GenericDataTypeID dataTypeID = GetDataTypeManager().GetGenericDataTypeIDByName(dataTypeStr);

				if (dataTypeID != GenericDataTypeID{})
				{

					Internal::SetVariableDataType(varID, aDataType.mVariableContainer, dataTypeID, nullptr);

					GetDataTypeManager().LoadData(dataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

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

		//void SaveStruct(const Struct& aStruct, std::string_view aFilePath)
		//{
		//	std::filesystem::path fileDirectory = aFilePath;
		//	std::filesystem::path filePath = fileDirectory.string() + "/" + aStruct.mName + FLY_FILE_EXTENSION;

		//	std::filesystem::create_directories(std::string(aFilePath));
		//	if (!std::filesystem::exists(fileDirectory))
		//	{
		//		throw std::runtime_error("Failed to create directory: " + fileDirectory.string());
		//		return;

		//	}

		//	std::ofstream ofs(filePath, std::ios::out);

		//	if (!ofs.is_open())
		//	{
		//		throw std::runtime_error("Failed to open file for writing: " + filePath.string());
		//		return;
		//	}


		//	nlohmann::json jsonDoc;

		//	jsonDoc["Type"] = "Struct";
		//	jsonDoc["Name"] = aStruct.mName;

		//	nlohmann::json& dataJson = jsonDoc["Data"];

		//	{
		//		dataJson["Variables"] = nlohmann::json::array();
		//		nlohmann::json& variableDataJson = dataJson["Variables"];

		//		for (VarID varID{ 0 }; varID < aStruct.mVariableContainer.mVariables.size(); ++varID)
		//		{
		//			const Variable& variable = aStruct.mVariableContainer.mVariables.at(varID);

		//			if (variable.mIsDestroyed)
		//			{
		//				continue;
		//			}

		//			nlohmann::json variableJson;

		//			variableJson["Name"] = variable.mName;
		//			variableJson["DataType"] = GetDataTypeManager().GetName(variable.mDataTypeID);

		//			nlohmann::json defaultValueJson = nlohmann::json::object();

		//			GetDataTypeManager().SaveData(variable.mDataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

		//			variableJson["DefaultValue"] = defaultValueJson;

		//			/*variableJson["Nodes"] = nlohmann::json::array();
		//			nlohmann::json& variableNodesJson = variableJson["Nodes"];

		//			for (const NodeRef& nodeRef : variableManager.GetNodeRefsByVarID(varID))
		//			{

		//				const Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

		//				if (!node.mIsDestroyed)
		//				{
		//					nlohmann::json& varNodeJson = variableNodesJson.emplace_back();
		//					const NodeID cleanNodeID = cleanedNodeIDs.at(nodeRef.GetNodeID());
		//					varNodeJson["NodeID"] = cleanNodeID;
		//					nlohmann::json& graphJson = varNodeJson["Graph"];
		//					assert(nodeRef.GetClass());
		//					graphJson["ClassName"] = nodeRef.GetClass()->mName;
		//					graphJson
		//					variableNodesJson.push_back(varNodeJson);
		//				}
		//			}*/

		//			variableDataJson.push_back(variableJson);
		//		}
		//	}

		//	ofs << jsonDoc;
		//}

		void LoadDataType(const nlohmann::json& aJsonData)
		{
			const std::string& structName = aJsonData["Name"];
			const DataTypeID createdStructID = Internal::CreateStruct(structName);
			LoadDataType(aJsonData["Data"], *GetDataTypeManager().Find(createdStructID));
		}

		//void LoadStruct(const nlohmann::json& aJsonData, Struct& aStruct)
		//{
		//	const nlohmann::json& dataJson = aJsonData;


		//	const nlohmann::json& variableDataJson = dataJson["Variables"];

		//	for (const nlohmann::json& variableJson : variableDataJson)
		//	{
		//		const VarID varID = Internal::CreateVariable(aStruct.mVariableContainer, GenericDataTypeID{ GetDataTypeID<bool>() }, "Var", nullptr);
		//		Variable& variable = aStruct.mVariableContainer.mVariables.at(varID);

		//		const std::string& dataTypeStr = variableJson["DataType"];

		//		const std::string variableName = variableJson["Name"];
		//		Internal::SetVariableName(varID, aStruct.mVariableContainer, variableName, nullptr);

		//		const nlohmann::json& defaultValueJson = variableJson["DefaultValue"];


		//		const GenericDataTypeID dataTypeID = GetDataTypeManager().GetGenericDataTypeIDByName(dataTypeStr);

		//		if (dataTypeID != GenericDataTypeID{})
		//		{

		//			Internal::SetVariableDataType(varID, aStruct.mVariableContainer, dataTypeID, nullptr);

		//			GetDataTypeManager().LoadData(dataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

		//		}

		//		/*const json& variableNodesJson = variableJson["Nodes"];

		//		for (const NodeID nodeID : variableNodesJson)
		//		{
		//			if (!failedNodeIDs.contains(nodeID))
		//			{
		//				InternalModifier::BindVariable(aScript, NodeRef{.nodeID = nodeID, .nodeGraph = , varID, nullptr);
		//			}
		//			else
		//			{
		//				std::cout << "Couldn't bind node to variable" << std::endl;
		//			}
		//		}*/
		//	}
		//}

		void SaveClass(const Class& aClass, const std::string_view aFilePath)
		{
			std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);
			std::filesystem::path filePath = fileDirectory.string() + aClass.mName + FLY_FILE_EXTENSION;

			std::filesystem::create_directories(std::string(aFilePath));
			if (!std::filesystem::exists(fileDirectory))
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

			const NodeGraph& eventGraph = aClass.mEventGraph.mNodeGraph;

			nlohmann::json jsonDoc;

			jsonDoc["Type"] = "Class";
			jsonDoc["Name"] = aClass.mName;
			jsonDoc["Target"] = GetDataTypeManager().GetName(aClass.mTargetID);

			nlohmann::json& dataJson = jsonDoc["Data"];
			std::unordered_map<NodeID, NodeID> cleanedNodeIDs;


			{
				dataJson["Nodes"] = nlohmann::json::array();

				nlohmann::json& nodesArrayJson = dataJson["Nodes"];


				for (NodeID nodeID{ 0 }; nodeID < eventGraph.mNodes.size(); ++nodeID)
				{
					const Node& node = eventGraph.mNodes.at(nodeID);
					if (node.mIsDestroyed)
					{
						continue;
					}

					nlohmann::json nodeJson;
					cleanedNodeIDs.emplace(nodeID, NodeID{ static_cast<NodeID::id_type>(cleanedNodeIDs.size()) });

					nodeJson["ID"] = cleanedNodeIDs.at(nodeID).mID;
					nodeJson["Name"] = GetNodeTypeManager().GetShortName(node.mTypeID);
					nodeJson["Pos"]["x"] = node.mPosition.x;
					nodeJson["Pos"]["y"] = node.mPosition.y;

					//const NodeType& nodeType = GetNodeTypeManager().GetNodeType(node.mTypeID);

					/*if (HasFlag(nodeType.mNodeRecipe.mTraits, eNodeTrait::Accessor))
					{
						const VarID varID = aClass.mVariableManager.GetVariableIDByNodeRef(CreateGlobalNodeRef(nodeID, eventGraph, aClass));

						varID;

					}*/

					nodesArrayJson.push_back(nodeJson);
				}
			}

			{
				dataJson["PinData"] = nlohmann::json::array();
				nlohmann::json& pinDataArrayJson = dataJson["PinData"];

				for (const PinID inputPinID : Internal::GetInputPins(eventGraph))
				{
					const Pin& pin = eventGraph.mPins.at(inputPinID);

					nlohmann::json pinDataJson;

					pinDataJson["NodeID"] = cleanedNodeIDs.at(pin.mNodeID).mID;
					pinDataJson["PinIndex"] = Internal::GetPinIndex(eventGraph, inputPinID);

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
							connectedPinJson["NodeID"] = cleanedNodeIDs.at(connectedPin.mNodeID).mID;
							connectedPinJson["PinIndex"] = Internal::GetPinIndex(eventGraph, connectedPinID);
						}

					}
					else
					{
						const PinType& pinType = GetPinTypeManager().GetPinType(pin.mTypeID);

						pinDataJson["DataType"] = GetDataTypeManager().GetName(pinType.mGenericDataTypeID);

						nlohmann::json valueJson = nlohmann::json::object();
						GetDataTypeManager().SaveData(pinType.mGenericDataTypeID, pin.mDataPtr, valueJson);
						pinDataJson["Value"] = valueJson;
					}

					pinDataArrayJson.push_back(pinDataJson);
				}
			}

			{
				dataJson["Variables"] = nlohmann::json::array();
				nlohmann::json& variableDataJson = dataJson["Variables"];

				for (VarID varID{ 0 }; varID < aClass.mVariableContainer.mVariables.size(); ++varID)
				{
					const Variable& variable = aClass.mVariableContainer.mVariables.at(varID);

					if (variable.mIsDestroyed)
					{
						continue;
					}

					nlohmann::json variableJson;

					variableJson["Name"] = variable.mName;
					variableJson["DataType"] = GetDataTypeManager().GetName(variable.mDataTypeID);

					nlohmann::json defaultValueJson = nlohmann::json::object();

					GetDataTypeManager().SaveData(variable.mDataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

					variableJson["DefaultValue"] = defaultValueJson;

					/*variableJson["Nodes"] = nlohmann::json::array();
					nlohmann::json& variableNodesJson = variableJson["Nodes"];

					for (const NodeRef& nodeRef : variableManager.GetNodeRefsByVarID(varID))
					{

						const Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

						if (!node.mIsDestroyed)
						{
							nlohmann::json& varNodeJson = variableNodesJson.emplace_back();
							const NodeID cleanNodeID = cleanedNodeIDs.at(nodeRef.GetNodeID());
							varNodeJson["NodeID"] = cleanNodeID;
							nlohmann::json& graphJson = varNodeJson["Graph"];
							assert(nodeRef.GetClass());
							graphJson["ClassName"] = nodeRef.GetClass()->mName;
							graphJson
							variableNodesJson.push_back(varNodeJson);
						}
					}*/

					variableDataJson.push_back(variableJson);
				}
			}

			ofs << jsonDoc;
			ofs.close();
		}

		void LoadClass(const nlohmann::json& aJsonData)
		{
			const std::string& className = aJsonData["Name"];
			const std::string& targetName = aJsonData["Target"];
			const ClassID createdClassID = Internal::CreateClass(GetDataTypeManager().GetGenericDataTypeIDByName(targetName), className);
			LoadClass(aJsonData["Data"], GetClassByID(createdClassID));
		}

		void LoadClass(const nlohmann::json& aJsonData, Class& aClass)
		{
			EventGraph& eventGraph = aClass.mEventGraph;
			NodeGraph& eventNodeGraph = eventGraph.mNodeGraph;

			const nlohmann::json& dataJson = aJsonData;

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

				const NodeID pinNodeID{ pinData["NodeID"] };
				if (failedNodeIDs.contains(pinNodeID))
				{
					continue;
				}
				const size_t pinIndex = pinData["PinIndex"];
				const PinID pinID = Internal::GetPinID(eventNodeGraph, pinNodeID, pinIndex, eFlowType::Input);

				if (pinID == InvalidID<PinID>())
				{
					continue;
				}
				Pin& pin = eventNodeGraph.mPins.at(pinID);
				const PinType& pinType = GetPinTypeManager().GetPinType(pin.mTypeID);

				const nlohmann::json& connectionJson = pinData["Connections"];
				const bool connectionExists = connectionJson["Exists"];
				if (connectionExists)
				{
					const NodeID connectionNodeID{ connectionJson["NodeID"] };
					if (failedNodeIDs.contains(connectionNodeID))
					{
						continue;
					}

					const size_t connectedPinIndex = connectionJson["PinIndex"];

					const PinID connectionID = Internal::GetPinID(eventNodeGraph, connectionNodeID, connectedPinIndex, eFlowType::Output);

					if (connectionID != InvalidID<PinID>())
					{
						Internal::TryCreateLink(eventNodeGraph, pinID, connectionID, nullptr);
					}
					continue;
				}


				const nlohmann::json& valueJson = pinData["Value"];
				GetDataTypeManager().LoadData(pinType.mGenericDataTypeID, pin.mDataPtr, valueJson);
			}

			const nlohmann::json& variableDataJson = dataJson["Variables"];

			for (const nlohmann::json& variableJson : variableDataJson)
			{
				const VarID varID = Internal::CreateVariable(aClass.mVariableContainer, GenericDataTypeID{ GetDataTypeID<bool>() }, "Var", nullptr);
				Variable& variable = aClass.mVariableContainer.mVariables.at(varID);

				const std::string& dataTypeStr = variableJson["DataType"];

				const std::string variableName = variableJson["Name"];
				Internal::SetVariableName(varID, aClass.mVariableContainer, variableName, nullptr);

				const nlohmann::json& defaultValueJson = variableJson["DefaultValue"];


				const GenericDataTypeID dataTypeID = GetDataTypeManager().GetGenericDataTypeIDByName(dataTypeStr);

				if (dataTypeID != GenericDataTypeID{})
				{

					Internal::SetVariableDataType(varID, aClass.mVariableContainer, dataTypeID, nullptr);

					GetDataTypeManager().LoadData(dataTypeID, variable.mDefaultValueDataPtr, defaultValueJson);

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

		void LoadAllFlyFiles(const std::string_view aFilePath)
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

					if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".fly")
					{
						std::cout << "Found script file: " << entry.path() << std::endl;

						const std::string fileName = entry.path().filename().string();

						LoadFlyFile(entry.path().string());
					}
				}
			}
		}

		void CreateCopyOfClass(const Class& aClass, const std::string_view aFilePath, const std::string_view aCopyName)
		{
			const std::filesystem::path fileDirectory = GetFileDirectory(aFilePath);
			std::string filePath = fileDirectory.string() + aClass.mName + FLY_FILE_EXTENSION;
			std::string copyPath = fileDirectory.string() + std::string(aCopyName) + FLY_FILE_EXTENSION;

			if (std::filesystem::copy_file(filePath, copyPath))
			{
				std::cout << "Succeeded copying file: " << filePath << std::endl;
			}
			else
			{
				throw std::runtime_error("Failed copying file");
			}
		}

		void SaveCustomEvents(const std::string_view aFilePath)
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

			const NodeTypeManager& nodeTypeManager = GetNodeTypeManager();

			const std::vector<CustomEvent>& customEventNodeTypes = nodeTypeManager.GetCustomEvents();

			for (const CustomEvent& customEventNodeType : customEventNodeTypes)
			{
				const NodeType& executorNodeType = GetNodeTypeManager().GetNodeType(customEventNodeType.GetExecutorTypeID());

				nlohmann::json customEventJson;

				customEventJson["Name"] = nodeTypeManager.GetShortName(customEventNodeType.GetExecutorTypeID());


				nlohmann::json pinArrayJson = nlohmann::json::array();

				for (size_t i = 1; i < executorNodeType.mNodeRecipe.mOutputPinTypeIDs.size(); ++i)
				{
					PinTypeID pinTypeID = executorNodeType.mNodeRecipe.mOutputPinTypeIDs[i];

					nlohmann::json customEventPinJson;

					const PinType& pinType = GetPinTypeManager().GetPinType(pinTypeID);

					customEventPinJson["Name"] = pinType.mName;
					customEventPinJson["DataType"] = GetDataTypeManager().GetName(pinType.mGenericDataTypeID);

					pinArrayJson.push_back(customEventPinJson);
				}

				customEventJson["Pins"] = pinArrayJson;

				customEventsJson.push_back(customEventJson);
			}

			jsonDoc["CustomEvents"] = customEventsJson;

			ofs << jsonDoc;

			ofs.close();
		}

		void LoadCustomEvents(const std::string_view aFilePath)
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

				const CustomEventID customEventID = Internal::CreateCustomEvent(nodeName);

				const nlohmann::json& pinsJson = customEventJson["Pins"];

				for (const nlohmann::json& pinJson : pinsJson)
				{
					const std::string& pinName = pinJson["Name"];
					const std::string& dataTypeName = pinJson["DataType"];

					const GenericDataTypeID dataTypeID = GetDataTypeManager().GetGenericDataTypeIDByName(dataTypeName);

					Internal::AddPinToCustomEvent(customEventID, dataTypeID, pinName, nullptr);
				}
			}
		}
	}
}